#ifndef {{ cookiecutter.project_slug.upper() }}_HPP
#define {{ cookiecutter.project_slug.upper() }}_HPP


#include <mcmc_simulation/header.hpp>


// Template implementation for a MCMC system


class {{ cookiecutter.project_name }};


struct {{ cookiecutter.project_name }}Parameters : public mcmc::simulation::SystemBaseParameters {
    explicit {{ cookiecutter.project_name }}Parameters(const json params):
            SystemBaseParameters(params),
            mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
            sigma(get_entry<double>("sigma", 0.4)),
            dt(get_entry<double>("dt", 0.01))
    {}

    {{ cookiecutter.project_name }}Parameters(const std::vector<double> mu_, const double sigma_, const double dt_) : {{ cookiecutter.project_name }}Parameters(json{
            {"mu", mu_},
            {"sigma", sigma_},
            {"dt", dt_}
    })
    {}

    std::unique_ptr<{{ cookiecutter.project_name }}> generate() { return std::make_unique<{{ cookiecutter.project_name }}>(*this); }

    const std::vector<double> mu;
    const double sigma;
    const double dt;
};


// Define which ones are optional...


{%- if cookiecutter.use_predefined_measures == "No" %}
class {{ cookiecutter.project_name }} : public mcmc::simulation::SystemBase<{{ cookiecutter.project_name }}>
{%- else %}
class {{ cookiecutter.project_name }} : public mcmc::simulation::MeasureSystemBase<{{ cookiecutter.project_name }}>
{%- endif %}
{
public:
    explicit {{ cookiecutter.project_name }}(const {{ cookiecutter.project_name }}Parameters &sp_) :
        sp(sp_),
        normal(std::normal_distribution<double>(0.0, 1.0))
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

{%- if cookiecutter.use_predefined_measures == "No" %}
    void initialize_measurements(std::string starting_mode, uint rep=1)
    {}

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        for(const auto measure_name: get_measure_names())
        {
            if(measure_name == "Mean")
            {
                auto mean = std::accumulate(system.begin(), system.end(), 0.0);
                results.push_back(mean / get_size());
            }
            else if(measure_name == "Config")
            {
                std::string config = std::to_string(system[0]);
                for (uint idx = 1; idx < get_size(); idx++)
                    config += ", " + std::to_string(system[idx]);
                results.push_back(config);
            }
        }
        return results;
    }
    
    void finalize_measurements(std::string starting_mode, uint rep=1)
    {}
{%- endif %}

    std::vector<std::string> get_measure_names()
    {
        return sp.get_measures();
    }

private:
    std::vector<double> system; // Or any other system representation
    std::normal_distribution<double> normal;

    const {{ cookiecutter.project_name }}Parameters &sp;
};

#endif //{{ cookiecutter.project_slug.upper() }}_HPP