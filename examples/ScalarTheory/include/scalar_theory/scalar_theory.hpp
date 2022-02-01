#ifndef MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
#define MCMCSIMULATIONLIB_SCALAR_THEORY_HPP


#include <mcmc_simulation/header.hpp>


// Example implementation of a scalar theory


class ScalarTheory;

class ScalarTheoryParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit ScalarTheoryParameters(const json params):
            SystemBaseParameters(params),
            kappa(get_entry<double>("kappa", 0.4)),
            lambda(get_entry<double>("lambda", 1.0)),
            dt(get_entry<double>("dt", 0.01)),
            n(get_entry<int>("n", 20)),
            m(get_entry<double>("m", 1.0)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))

    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }
    }

    ScalarTheoryParameters(const double kappa_, const double lambda_, std::vector<int> dimensions_,
        const double dt_, const int n_, const double m_=1.0) : ScalarTheoryParameters(json{
            {"kappa", kappa_},
            {"lambda", lambda_},
            {"dimensions", dimensions_},
            {"dt", dt_},
            {"n",  n_},
            {"m", m_}
    })
    {}

    std::unique_ptr<ScalarTheory> generate() { return std::make_unique<ScalarTheory>(*this); }

private:
    friend class ScalarTheory;

    const double kappa;
    const double lambda;
    const double dt;
    const int n;
    const double m;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};


class ScalarTheory : public mcmc::simulation::SystemBase<ScalarTheory>
{
public:
    explicit ScalarTheory(const ScalarTheoryParameters &stsp_) :
        stsp(stsp_),
        normal(std::normal_distribution<double>(0.0, 1.0)),
        rand(std::uniform_real_distribution<double> (0,1)),
        momenta(std::vector<double>(get_size(), 0.0)),
        current_momenta(std::vector<double>(get_size(), 0.0))
    {
        neighbours = std::vector<std::vector<int>> (get_size(), std::vector<int>(2 * stsp.dimensions.size()));
        for(uint site_idx = 0; site_idx < get_size(); site_idx++)
        {
            for(uint d = 0; d < stsp.dimensions.size(); d++)
            {
                neighbours[site_idx][2 * d] = neigh_dir(site_idx, d, true);
                neighbours[site_idx][2 * d + 1] = neigh_dir(site_idx, d, false);
            }
        }
    }

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
        for(auto i = 0; i < measure_interval; i++)
        {
            auto current_action = action();
            auto current_lattice(lattice);

            // Sample momenta
            std::generate(momenta.begin(), momenta.end(), [this]() { return normal(mcmc::util::gen); });
            std::copy(momenta.begin(), momenta.end(), current_momenta.begin());

            // Hamiltonians' equation - Leapfrog
            for(auto n = 0; n < stsp.n; n++)
            {
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta[site_idx] -= stsp.dt / 2.0 * drift(site_idx);
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    lattice[site_idx] += stsp.dt * momenta[site_idx] / stsp.m;
                for (uint site_idx = 0; site_idx < get_size(); site_idx++)
                    momenta[site_idx] -= stsp.dt  / 2.0 * drift(site_idx);
            }

            auto proposal_action = action();

            auto current_kinetic_term = std::inner_product(current_momenta.begin(), current_momenta.end(), current_momenta.begin(), 0.0);
            auto proposal_kinetic_term = std::inner_product(momenta.begin(), momenta.end(), momenta.begin(), 0.0);

            // std::cout << proposal_action + 0.5 / stsp.m * proposal_kinetic_term << " == " << current_action + 0.5 / stsp.m * current_kinetic_term << std::endl;

            // Accept/Reject step
            if (rand(mcmc::util::gen) >= std::min(1.0, std::exp(
                    -1.0 * (proposal_action - current_action) - 0.5 * (proposal_kinetic_term - current_kinetic_term) / stsp.m))) {
                lattice = current_lattice; // Reject
            }
            // else{} // Accept
        }
    }

    uint16_t get_size() const
    {
        return stsp.n_sites;
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
            for(uint d = 0; d < stsp.dimensions.size(); d++)
                kinetic_term += lattice[neighbours[site_idx][2 * d]];
            action_ += -2.0 * stsp.kappa * lattice[site_idx] * kinetic_term + \
                (1.0 - 2.0 * stsp.lambda) * std::pow(lattice[site_idx], 2.0) + stsp.lambda * std::pow(lattice[site_idx], 4.0);
        }
        return action_;
    }

    double drift(const uint site_idx)
    {
        double drift_ = 0;
        for(uint d = 0; d < stsp.dimensions.size(); d++)
            drift_ += lattice[neighbours[site_idx][2 * d]] + lattice[neighbours[site_idx][2 * d + 1]];
        drift_ = -2.0 * stsp.kappa * drift_  + \
            2.0 * (1.0 - 2.0 * stsp.lambda) * lattice[site_idx] + 4.0 * stsp.lambda * std::pow(lattice[site_idx], 3.0);
        return drift_;
    }

    /* void initialize_measurements(std::string starting_mode, uint rep=1)
    {} */

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        // std::vector<std::string> results_str;
        for(const auto measure_name: get_measure_names())
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
            else if(measure_name == "Config")
                results.push_back(configuration());
        }
        /* std::transform(results.begin(), results.end(), results_str.begin(), []
            (std::variant<int, double, std::string>& val)
            { return std::visit(
                overload{
                    [](const int &va)       { return std::to_string(va); },
                    [](const double &va)   { return std::to_string(va); },
                    [](const std::string &va)   { return va; }
                }, val);
            }); */
        return results;
    }

    /* void finalize_measurements(std::string starting_mode, uint rep=1)
    {} */

    std::vector<std::string> get_measure_names()
    {
        return stsp.get_measures();
    }

private:
    std::vector<double> lattice;
    std::vector<std::vector<int>> neighbours;

    const ScalarTheoryParameters &stsp;

    std::vector<double> momenta;
    std::vector<double> current_momenta;
    std::normal_distribution<double> normal;
    std::uniform_real_distribution<double> rand;

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
    int neigh_dir(int n, int d, bool dir) const {
        if(dir)
            return n-n%(stsp.dim_mul[d]*stsp.dimensions[d])+(n+stsp.dim_mul[d])%(stsp.dim_mul[d]*stsp.dimensions[d]);
        else
            return n-n%(stsp.dim_mul[d]*stsp.dimensions[d])+(n-stsp.dim_mul[d]+stsp.dim_mul[d]*stsp.dimensions[d])%(stsp.dim_mul[d]*stsp.dimensions[d]);
    }
};

#endif //MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
