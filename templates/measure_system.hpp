#ifndef MCMCMEASURESYSTEM_HPP
#define MCMCMEASURESYSTEM_HPP


#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/random.hpp>


// Template implementation for a MCMC measure system


class MCMCMeasureSystem : public mcmc::simulation::MeasureSystemBase<MCMCMeasureSystem>
{
public:
    explicit MCMCMeasureSystem(const json params):
            MeasureSystemBase(params),
            // Configuration parameters
            mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
            sigma(get_entry<double>("sigma", 0.4)),
            dt(get_entry<double>("dt", 0.01)),
            
            // Further member variables
            normal(std::normal_distribution<double>(0.0, 1.0))
    {}

    MCMCMeasureSystem(const std::vector<double> mu_={0.0, 1.0}, const double sigma_=0.4, const double dt_=0.01) : MCMCMeasureSystem(json{
            {"mu", mu_},
            {"sigma", sigma_},
            {"dt", dt_}
    })
    {}

    void initialize(std::string starting_mode)
    {
        // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
        std::transform(sp.mu.begin(), sp.mu.end(), std::back_inserter(system), [this] (const double param) -> double { return this->normal(mcmc::util::gen); });
    }

    void update_step(uint measure_interval=1)
    {
        // Called every MCMC step
        for(auto i = 0; i < get_size(); i++)
            system[i] = system[i] - sp.dt * (system[i] - sp.mu[i]) / std::pow(sp.sigma, 2.0) + std::sqrt(2.0 * sp.dt) * normal(mcmc::util::gen);
        
    }

    uint16_t get_size() const
    {
        // Returns the size of the system, for example,
        return system.size();
    }

    auto at(int i) const
    {
        // Read system state i of the system representation, for example,
        return system[i];
    }

    auto& at(int i)
    {
        // Return system state i of the system representation, for example,
        return system[i];
    }

    auto get_system_representation() const
    {
        // Returns the entire MCMC system representation, for example,
        return system;
    }

    auto& get_system_representation()
    {
        // Returns the entire MCMC system representation, for example,
        return system;
    }

private:
    std::vector<double> mu;
    double sigma;
    double dt;
    
    std::vector<double> system; // Or any other system representation
    std::normal_distribution<double> normal;
};

#endif //MCMCMEASURESYSTEM_HPP