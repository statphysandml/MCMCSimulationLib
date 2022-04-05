#ifndef {{ cookiecutter.project_slug.upper() }}_HPP
#define {{ cookiecutter.project_slug.upper() }}_HPP


#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/random.hpp>


{%- if cookiecutter.use_predefined_measures == "No" %}
// Template implementation for a MCMC system
{%- else %}
// Template implementation for a MCMC measure system
{%- endif %}


{%- if cookiecutter.use_predefined_measures == "No" %}
class {{ cookiecutter.project_name }} : public mcmc::simulation::SystemBase<{{ cookiecutter.project_name }}>
{%- else %}
class {{ cookiecutter.project_name }} : public mcmc::simulation::MeasureSystemBase<{{ cookiecutter.project_name }}>
{%- endif %}
{
public:
    explicit {{ cookiecutter.project_name }}(const json params):
{%- if cookiecutter.use_predefined_measures == "No" %}
            SystemBase(params),
{%- else %}
            MeasureSystemBase(params),
{%- endif %}
            // Configuration parameters
            mu_(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
            sigma_(get_entry<double>("sigma", 0.4)),
            dt_(get_entry<double>("dt", 0.01)),
            
            // Further member variables
            normal_(std::normal_distribution<double>(0.0, 1.0)),
            system_(std::vector<double>(mu_.size(), 0.0))
    {}

    {{ cookiecutter.project_name }}(const std::vector<double> mu={0.0, 1.0}, const double sigma=0.4, const double dt=0.01) : {{ cookiecutter.project_name }}(json{
            {"mu", mu},
            {"sigma", sigma},
            {"dt", dt}
    })
    {}

    void initialize(std::string starting_mode)
    {
{%- if cookiecutter.use_predefined_measures == "Yes" %}
        // It is important to generate the predefined measures based on the virtual method of the base class - Note that this method can,
        // of course also be overloaded.
        generate_measures(measures_);
{%- endif %}

        // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
        if(starting_mode == "hot")
            std::transform(mu_.begin(), mu_.end(), system_.begin(), [this] (const double param) -> double { return param + this->sigma_ * this->normal_(mcmc::util::g_gen); });
        else
            std::fill(system_.begin(), system_.end(), 0);
    }

    void update_step(uint measure_interval=1)
    {
        // Called every MCMC step
        for(auto i = 0; i < get_size(); i++)
            system_[i] = system_[i] - dt_ * (system_[i] - mu_[i]) / std::pow(sigma_, 2.0) + std::sqrt(2.0 * dt_) * normal_(mcmc::util::g_gen);
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

{%- if cookiecutter.use_predefined_measures == "No" %}
    void initialize_measurements(std::string starting_mode, uint rep=1)
    {}

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        for(const auto measure_name: this->measure_names())
        {
            if(measure_name == "Mean")
            {
                auto mean = std::accumulate(system_.begin(), system_.end(), 0.0);
                results.push_back(mean / get_size());
            }
            else if(measure_name == "Config")
            {
                std::string config = std::to_string(system_[0]);
                for (uint idx = 1; idx < get_size(); idx++)
                    config += ", " + std::to_string(system_[idx]);
                results.push_back(config);
            }
        }
        return results;
    }
    
    void finalize_measurements(std::string starting_mode, uint rep=1)
    {}
{%- endif %}

private:
    std::vector<double> mu_;
    double sigma_;
    double dt_;

    std::vector<double> system_; // Or any other system representation
    std::normal_distribution<double> normal_;
};

#endif //{{ cookiecutter.project_slug.upper() }}_HPP