#ifndef MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
#define MCMCSIMULATIONLIB_SCALAR_THEORY_HPP


#include <mcmc_simulation/header.hpp>


// Template implementation for a MCMC system


struct SystemParameters : public mcmc::simulation::SystemBaseParameters {
    explicit SystemParameters(const json params):
            SystemBaseParameters(params),
            param_a(get_entry<double>("param_b", 0.4)),
            param_b(get_entry<std::vector<double>>("param_b", {0.0, 1.0})),
    {}

    SystemParameters(const double param_a_, const std::vector<double> param_b_) : SystemParameters(json{
            {"param_a", param_a_},
            {"param_b", param_b_},
    })
    {}

    std::unique_ptr<System> generate() { return std::make_unique<System>(*this); }

    const double param_a;
    const std::vector<double> param_b;
};


// Define which ones are optional...


class System : public mcmc::simulation::SystemBase<System>
{
public:
    explicit System(const SystemParameters &sp_) :
        sp(sp_),
    {}

    void initialize(std::string starting_mode)
    {
        // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold".
    }

    void update_step(uint measure_interval=1)
    {
        // Called every MCMC step
    }

    uint16_t get_size() const
    {
        // Returns the size of the system
    }

    auto at(int i) const
    {
        // Read system state i of the system representation
    }

    auto& at(int i)
    {
        // Return system state i of the system representation
    }

    auto get_system_representation() const
    {
        // Returns the entire MCMC system representation
    }

    auto& get_system_representation()
    {
        // Returns the entire MCMC system representation
    }

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        return results;
    }

    std::vector<std::string> get_measure_names()
    {
        return sp.get_measures();
    }

private:
    std::vector<double> system; // Or any other system representation

    const SystemParameters &sp;
};

#endif //MCMCSIMULATIONLIB_SCALAR_THEORY_HPP
