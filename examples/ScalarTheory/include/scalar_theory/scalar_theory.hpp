#ifndef MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
#define MCMCSIMULATIONLIB_SCALAR_THEORY_HPP


#include <mcmc/mcmc_simulation/header.hpp>
#include <mcmc/mcmc_simulation/util/random.hpp>


// Example implementation of a scalar theory


class ScalarTheory : public mcmc::simulation::SystemBase<ScalarTheory>
{
public:
    explicit ScalarTheory(const json params):
        SystemBase(params),
        // Configuration parameters
        kappa_(get_entry<double>("kappa", 0.4)),
        lambda_(get_entry<double>("lambda", 1.0)),
        dt_(get_entry<double>("dt", 0.1)),
        n_(get_entry<int>("n", 10)),
        m_(get_entry<double>("m", 1.0)),
        dimensions_(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4})),

        // Further member variables
        normal_(std::normal_distribution<double>(0.0, 1.0)),
        rand_(std::uniform_real_distribution<double> (0,1)),
        acceptance_rate_(0.0),
        energy_violation_(0.0),
        exponential_energy_violation_(0.0)
    {
        n_sites_ = 1;
        dim_mul_.push_back(n_sites_);
        for(auto dim: dimensions_) {
            n_sites_ *= dim;
            dim_mul_.push_back(n_sites_);
        }

        neighbour_indices_ = std::vector<std::vector<int>> (get_size(), std::vector<int>(2 * dimensions_.size()));
        for(uint site_idx = 0; site_idx < get_size(); site_idx++)
        {
            for(uint d = 0; d < dimensions_.size(); d++)
            {
                neighbour_indices_[site_idx][2 * d] = neigh_dir(site_idx, d, true);
                neighbour_indices_[site_idx][2 * d + 1] = neigh_dir(site_idx, d, false);
            }
        }

        momenta_ = std::vector<double>(get_size(), 0.0);
        current_momenta_ = std::vector<double>(get_size(), 0.0);
    }

    ScalarTheory(const double kappa=0.4, const double lambda=1.0, std::vector<int> dimensions={4, 4},
        const double dt=0.1, const int n=10, const double m=1.0) : ScalarTheory(json{
            {"kappa", kappa},
            {"lambda", lambda},
            {"dimensions", dimensions},
            {"dt", dt},
            {"n",  n},
            {"m", m}
    })
    {}

    void initialize(std::string starting_mode)
    {        
        lattice_ = std::vector<double> (get_size(), 3.0);
        if(starting_mode == "hot")
        {
            for(auto &site : lattice_)
                site = normal_(mcmc::util::g_gen);
        }
    }

    // Hamiltonian Monte Carlo
    void update_step(uint measure_interval=1)
    {
        int n_accepted = 0;
        double energy_violation = 0;
        double exponential_energy_violation = 0;

        for(auto i = 0; i < measure_interval; i++)
        {
            auto current_action = action();
            auto current_lattice(lattice_);

            // Sample momenta
            std::generate(momenta_.begin(), momenta_.end(), [this]() { return normal_(mcmc::util::g_gen); });
            std::copy(momenta_.begin(), momenta_.end(), current_momenta_.begin());

            // Hamiltonians' equation - Leapfrog
            for(auto j = 0; j < n_; j++)
            {
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta_[site_idx] -= dt_ / 2.0 * drift(site_idx);
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    lattice_[site_idx] += dt_ * momenta_[site_idx] / m_;
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta_[site_idx] -= dt_ / 2.0 * drift(site_idx);
            }

            auto proposal_action = action();

            auto current_kinetic_term = std::inner_product(current_momenta_.begin(), current_momenta_.end(), current_momenta_.begin(), 0.0);
            auto proposal_kinetic_term = std::inner_product(momenta_.begin(), momenta_.end(), momenta_.begin(), 0.0);

            // std::cout << proposal_action + 0.5 / m * proposal_kinetic_term << " == " << current_action + 0.5 / m * current_kinetic_term << std::endl;

            auto energy_difference = 1.0 * (proposal_action - current_action) + 0.5 * (proposal_kinetic_term - current_kinetic_term) / m_;

            energy_violation += std::abs(energy_difference);
            exponential_energy_violation += std::exp(-1.0 * energy_difference);

            // Accept/Reject step
            if (rand_(mcmc::util::g_gen) >= std::min(1.0, std::exp(-1.0 * energy_difference))) {
                lattice_ = current_lattice; // Reject
            }
            else {
                n_accepted += 1; // Accept
            }
        }
        energy_violation_ = energy_violation / measure_interval;
        exponential_energy_violation_ = exponential_energy_violation / measure_interval;
        acceptance_rate_ = n_accepted * 1.0 / measure_interval;
    }

    uint16_t get_size() const
    {
        return n_sites_;
    }

    auto at(int i) const
    {
        return lattice_[i];
    }

    auto& at(int i)
    {
        return lattice_[i];
    }

    auto get_system_representation() const
    {
        return lattice_;
    }

    auto& get_system_representation()
    {
        return lattice_;
    }

    double action()
    {
        double action = 0;
        for(uint site_idx = 0; site_idx < get_size(); site_idx++)
        {
            double kinetic_term = 0;
            for(uint d = 0; d < dimensions_.size(); d++)
                kinetic_term += lattice_[neighbour_indices_[site_idx][2 * d]];
            action += -2.0 * kappa_ * lattice_[site_idx] * kinetic_term + \
                (1.0 - 2.0 * lambda_) * std::pow(lattice_[site_idx], 2.0) + lambda_ * std::pow(lattice_[site_idx], 4.0);
        }
        return action;
    }

    double drift(const uint site_idx)
    {
        double drift = 0;
        for(uint d = 0; d < dimensions_.size(); d++)
            drift += lattice_[neighbour_indices_[site_idx][2 * d]] + lattice_[neighbour_indices_[site_idx][2 * d + 1]];
        drift = -2.0 * kappa_ * drift  + \
            2.0 * (1.0 - 2.0 * lambda_) * lattice_[site_idx] + 4.0 * lambda_ * std::pow(lattice_[site_idx], 3.0);
        return drift;
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
                results.push_back(acceptance_rate_);
            else if(measure_name == "EnergyViolation")
                results.push_back(energy_violation_);
            else if(measure_name == "ExponentialEnergyViolation")
                results.push_back(exponential_energy_violation_);
            else if(measure_name == "Config")
                results.push_back(configuration());
        }
        return results;
    }

    void finalize_measurements(std::string starting_mode, uint rep=1)
    {}

private:
    double kappa_;
    double lambda_;
    double dt_;
    int n_;
    double m_;

    uint16_t n_sites_; // Total number of sites
    std::vector<int> dimensions_; // Different dimensions
    std::vector<int> dim_mul_; // Accumulated different dimensions (by product)

    std::vector<double> lattice_;
    std::vector<std::vector<int>> neighbour_indices_;

    std::vector<double> momenta_;
    std::vector<double> current_momenta_;
    std::normal_distribution<double> normal_;
    std::uniform_real_distribution<double> rand_;

    double acceptance_rate_;
    double energy_violation_;
    double exponential_energy_violation_;

    double mean()
    {
        auto mean = std::accumulate(lattice_.begin(), lattice_.end(), 0.0);
        return mean / get_size();
    }

    double abs_mean()
    {
        return std::abs(mean());
    }

    double second_moment()
    {
        auto second_moment = std::inner_product(lattice_.begin(), lattice_.end(), lattice_.begin(), 0.0);
        return second_moment / get_size();
    }

    double fourth_moment()
    {
        auto fourth_moment = std::accumulate(lattice_.begin(), lattice_.end(), 0.0, [] (const double &sum, const double &a) { return sum + std::pow(a, 4.0); });
        return fourth_moment / get_size();
    }

    std::string configuration()
    {
        std::string config = std::to_string(lattice_[0]);
        for (uint site_idx = 1; site_idx < get_size(); site_idx++)
            config += ", " + std::to_string(lattice_[site_idx]);
        return config;
    }

    //site, moving dimension, direction
    int neigh_dir(int i, int d, bool dir) const {
        if(dir)
            return i-i%(dim_mul_[d]*dimensions_[d])+(i+dim_mul_[d])%(dim_mul_[d]*dimensions_[d]);
        else
            return i-i%(dim_mul_[d]*dimensions_[d])+(i-dim_mul_[d]+dim_mul_[d]*dimensions_[d])%(dim_mul_[d]*dimensions_[d]);
    }
};

#endif //MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
