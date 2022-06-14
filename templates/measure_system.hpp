#ifndef MCMCMEASURESYSTEM_HPP
#define MCMCMEASURESYSTEM_HPP


#include <mcmc/mcmc_simulation/header.hpp>
#include <mcmc/mcmc_simulation/util/random.hpp>


// Template implementation for a MCMC measure system


class MCMCMeasureSystem : public mcmc::simulation::MeasureSystemBase<MCMCMeasureSystem>
{
public:
    explicit MCMCMeasureSystem(const json params):
            MeasureSystemBase(params),
            // Configuration parameters
            mu_(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
            sigma_(get_entry<double>("sigma", 0.4)),
            dt_(get_entry<double>("dt", 0.01)),
            
            // Further member variables
            normal_(std::normal_distribution<double>(0.0, 1.0)),
            system_(std::vector<double>(mu_.size(), 0.0))
    {}

    MCMCMeasureSystem(const std::vector<double> mu={0.0, 1.0}, const double sigma=0.4, const double dt=0.01) : MCMCMeasureSystem(json{
            {"mu", mu},
            {"sigma", sigma},
            {"dt", dt}
    })
    {}

    void initialize(std::string starting_mode)
    {
        // It is important to generate the predefined measures based on the virtual method of the base class - Note that this method can,
        // of course also be overloaded.
        generate_measures(measures_);

        // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
        if(starting_mode == "hot")
            std::transform(mu_.begin(), mu_.end(), system_.begin(), [this] (const double param) -> double { return param + this->sigma_ * this->normal_(mcmc::util::random::g_gen); });
        else
            std::fill(system_.begin(), system_.end(), 0);
    }

    void update_step(uint measure_interval=1)
    {
        // Called every MCMC step
        for(auto i = 0; i < get_size(); i++)
            system_[i] = system_[i] - dt_ * (system_[i] - mu_[i]) / std::pow(sigma_, 2.0) + std::sqrt(2.0 * dt_) * normal_(mcmc::util::random::g_gen);   
    }

    uint16_t get_size() const
    {
        // Returns the size of the system, for example,
        return system_.size();
    }

    auto at(int i) const
    {
        // Read system state i of the system representation, for example,
        return system_[i];
    }

    auto& at(int i)
    {
        // Return system state i of the system representation, for example,
        return system_[i];
    }

    auto get_system_representation() const
    {
        // Returns the entire MCMC system representation, for example,
        return system_;
    }

    auto& get_system_representation()
    {
        // Returns the entire MCMC system representation, for example,
        return system_;
    }

private:
    std::vector<double> mu_;
    double sigma_;
    double dt_;

    std::vector<double> system_; // Or any other system representation
    std::normal_distribution<double> normal_;
};

#endif //MCMCMEASURESYSTEM_HPP