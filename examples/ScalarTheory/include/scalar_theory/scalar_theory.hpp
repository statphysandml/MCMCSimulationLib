#ifndef MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
#define MCMCSIMULATIONLIB_SCALAR_THEORY_HPP


#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/random.hpp>


// Example implementation of a scalar theory


class ScalarTheory : public mcmc::simulation::SystemBase<ScalarTheory>
{
public:
    explicit ScalarTheory(const json params):
        SystemBase(params),
        // Configuration parameters
        kappa(get_entry<double>("kappa", 0.4)),
        lambda(get_entry<double>("lambda", 1.0)),
        dt(get_entry<double>("dt", 0.1)),
        n(get_entry<int>("n", 10)),
        m(get_entry<double>("m", 1.0)),
        dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4})),

        // Further member variables
        normal(std::normal_distribution<double>(0.0, 1.0)),
        rand(std::uniform_real_distribution<double> (0,1)),
        acceptance_rate(0.0),
        energy_violation(0.0),
        exponential_energy_violation(0.0)
    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }

        neighbour_indices = std::vector<std::vector<int>> (get_size(), std::vector<int>(2 * dimensions.size()));
        for(uint site_idx = 0; site_idx < get_size(); site_idx++)
        {
            for(uint d = 0; d < dimensions.size(); d++)
            {
                neighbour_indices[site_idx][2 * d] = neigh_dir(site_idx, d, true);
                neighbour_indices[site_idx][2 * d + 1] = neigh_dir(site_idx, d, false);
            }
        }

        momenta = std::vector<double>(get_size(), 0.0);
        current_momenta = std::vector<double>(get_size(), 0.0);
    }

    ScalarTheory(const double kappa_=0.4, const double lambda_=1.0, std::vector<int> dimensions_={4, 4},
        const double dt_=0.1, const int n_=10, const double m_=1.0) : ScalarTheory(json{
            {"kappa", kappa_},
            {"lambda", lambda_},
            {"dimensions", dimensions_},
            {"dt", dt_},
            {"n",  n_},
            {"m", m_}
    })
    {}

    void initialize(std::string starting_mode)
    {        
        lattice = std::vector<double> (get_size(), 3.0);
        if(starting_mode == "hot")
        {
            for(auto &site : lattice)
                site = normal(mcmc::util::gen);
        }
    }

    // Hamiltonian Monte Carlo
    void update_step(uint measure_interval=1)
    {
        int n_accepted = 0;
        double energy_violation_ = 0;
        double exponential_energy_violation_ = 0;

        for(auto i = 0; i < measure_interval; i++)
        {
            auto current_action = action();
            auto current_lattice(lattice);

            // Sample momenta
            std::generate(momenta.begin(), momenta.end(), [this]() { return normal(mcmc::util::gen); });
            std::copy(momenta.begin(), momenta.end(), current_momenta.begin());

            // Hamiltonians' equation - Leapfrog
            for(auto j = 0; j < n; j++)
            {
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta[site_idx] -= dt / 2.0 * drift(site_idx);
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    lattice[site_idx] += dt * momenta[site_idx] / m;
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta[site_idx] -= dt / 2.0 * drift(site_idx);
            }

            auto proposal_action = action();

            auto current_kinetic_term = std::inner_product(current_momenta.begin(), current_momenta.end(), current_momenta.begin(), 0.0);
            auto proposal_kinetic_term = std::inner_product(momenta.begin(), momenta.end(), momenta.begin(), 0.0);

            // std::cout << proposal_action + 0.5 / m * proposal_kinetic_term << " == " << current_action + 0.5 / m * current_kinetic_term << std::endl;

            auto energy_difference = 1.0 * (proposal_action - current_action) + 0.5 * (proposal_kinetic_term - current_kinetic_term) / m;

            energy_violation_ += std::abs(energy_difference);
            exponential_energy_violation_ += std::exp(-1.0 * energy_difference);

            // Accept/Reject step
            if (rand(mcmc::util::gen) >= std::min(1.0, std::exp(-1.0 * energy_difference))) {
                lattice = current_lattice; // Reject
            }
            else {
                n_accepted += 1; // Accept
            }
        }
        energy_violation = energy_violation_ / measure_interval;
        exponential_energy_violation = exponential_energy_violation_ / measure_interval;
        acceptance_rate = n_accepted * 1.0 / measure_interval;
    }

    uint16_t get_size() const
    {
        return n_sites;
    }

    auto at(int i) const
    {
        return lattice[i];
    }

    auto& at(int i)
    {
        return lattice[i];
    }

    auto get_system_representation() const
    {
        return lattice;
    }

    auto& get_system_representation()
    {
        return lattice;
    }

    double action()
    {
        double action_ = 0;
        for(uint site_idx = 0; site_idx < get_size(); site_idx++)
        {
            double kinetic_term = 0;
            for(uint d = 0; d < dimensions.size(); d++)
                kinetic_term += lattice[neighbour_indices[site_idx][2 * d]];
            action_ += -2.0 * kappa * lattice[site_idx] * kinetic_term + \
                (1.0 - 2.0 * lambda) * std::pow(lattice[site_idx], 2.0) + lambda * std::pow(lattice[site_idx], 4.0);
        }
        return action_;
    }

    double drift(const uint site_idx)
    {
        double drift_ = 0;
        for(uint d = 0; d < dimensions.size(); d++)
            drift_ += lattice[neighbour_indices[site_idx][2 * d]] + lattice[neighbour_indices[site_idx][2 * d + 1]];
        drift_ = -2.0 * kappa * drift_  + \
            2.0 * (1.0 - 2.0 * lambda) * lattice[site_idx] + 4.0 * lambda * std::pow(lattice[site_idx], 3.0);
        return drift_;
    }

    void initialize_measurements(std::string starting_mode, uint rep=1)
    {}

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        for(const auto measure_name: this->measure_names())
        {
            if(measure_name == "Mean")
                results.push_back(mean());
            else if(measure_name == "AbsMean")
                results.push_back(abs_mean());
            else if(measure_name == "SecondMoment")
                results.push_back(second_moment());
            else if(measure_name == "FourthMoment")
                results.push_back(fourth_moment());
            else if(measure_name == "Action")
                results.push_back(action() / get_size());
            else if(measure_name == "AcceptanceRate")
                results.push_back(acceptance_rate);
            else if(measure_name == "EnergyViolation")
                results.push_back(energy_violation);
            else if(measure_name == "ExponentialEnergyViolation")
                results.push_back(exponential_energy_violation);
            else if(measure_name == "Config")
                results.push_back(configuration());
        }
        return results;
    }

    void finalize_measurements(std::string starting_mode, uint rep=1)
    {}

private:
    double kappa;
    double lambda;
    double dt;
    int n;
    double m;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)

    std::vector<double> lattice;
    std::vector<std::vector<int>> neighbour_indices;

    std::vector<double> momenta;
    std::vector<double> current_momenta;
    std::normal_distribution<double> normal;
    std::uniform_real_distribution<double> rand;

    double acceptance_rate;
    double energy_violation;
    double exponential_energy_violation;

    double mean()
    {
        auto mean = std::accumulate(lattice.begin(), lattice.end(), 0.0);
        return mean / get_size();
    }

    double abs_mean()
    {
        return std::abs(mean());
    }

    double second_moment()
    {
        auto second_moment = std::inner_product(lattice.begin(), lattice.end(), lattice.begin(), 0.0);
        return second_moment / get_size();
    }

    double fourth_moment()
    {
        auto fourth_moment = std::accumulate(lattice.begin(), lattice.end(), 0.0, [] (const double &sum, const double &a) { return sum + std::pow(a, 4.0); });
        return fourth_moment / get_size();
    }

    std::string configuration()
    {
        std::string config = std::to_string(lattice[0]);
        for (uint site_idx = 1; site_idx < get_size(); site_idx++)
            config += ", " + std::to_string(lattice[site_idx]);
        return config;
    }

    //site, moving dimension, direction
    int neigh_dir(int i, int d, bool dir) const {
        if(dir)
            return i-i%(dim_mul[d]*dimensions[d])+(i+dim_mul[d])%(dim_mul[d]*dimensions[d]);
        else
            return i-i%(dim_mul[d]*dimensions[d])+(i-dim_mul[d]+dim_mul[d]*dimensions[d])%(dim_mul[d]*dimensions[d]);
    }
};

#endif //MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
