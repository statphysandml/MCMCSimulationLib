#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>
//#include <json.hpp>

//using json = nlohmann::json;

#include "param_helper/params.hpp"
#include "measure_policy.hpp"


class SystemBaseParameters : public impl_helper::params::Parameters {
public:
    using Parameters::Parameters;

    // Enables execution modes to temporarily use their own measures -> further good argument to add measures to system base parameters
    virtual void update_measures(const json& measures_)
    {}

    // Default parameter name for system parameters
    static const std::string name()
    {
        return "systembase_params";
    }

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, name());
    }

    // Function that enables to add parameters of other used classes with parameters
    virtual impl_helper::params::Parameters build_expanded_raw_parameters() const
    {
        Parameters parameters(params);
        return parameters;
    }

    // Implement this function with your respective "SystemClass" as "std::unique_ptr<SystemClass> generate() { return std::make_unique<SystemClass>(*this); }"
    template<typename System>
    std::unique_ptr<System> generate() {
        std::cout << "This function needs to be defined for your associate system class." << std::endl;
        std::exit(EXIT_FAILURE);
    }
};

template <typename Derived>
class SystemBase
{
public:
    void update(uint measure_interval=1)
    {
        systembase().update_step(measure_interval);
    }

    void init(std::string starting_mode)
    {
        systembase().initialize(starting_mode);
    }

    std::vector<std::string> measure()
    {
        return systembase().perform_measure();
    }

    std::vector<std::string> measure_names()
    {
        return systembase().get_measure_names();
    }

    const auto size() const
    {
        return systembase().get_size();
    }

    const auto operator[](int i) const
    {
        return systembase().at(i);
    }

    auto& operator[](int i)
    {
        return systembase().at(i);
    }

    const auto operator()() const
    {
        return systembase().get_system_representation();
    }

    auto& operator()()
    {
        return systembase().get_system_representation();
    }

    // Predefined measure method

    virtual std::vector<std::string> perform_measure()
    {
        std::vector<std::string> results;
        for(auto const& element: measures) {
            results.push_back(element->measure(systembase()));
        }
        return results;
    }

    virtual std::vector<std::string> get_measure_names()
    {
        std::vector<std::string> measure_names;
        for(auto const& element: measures) {
            measure_names.push_back(element->name());
        }
        return measure_names;
    }

//    virtual void save_config(int i) = 0;

protected:
    std::vector< std::unique_ptr<common_measures::MeasurePolicy<Derived>> > measures;

    void concat_measures(std::vector< std::unique_ptr<common_measures::MeasurePolicy<Derived>> >& measures_);

private:
    Derived& systembase() {
        return *static_cast<Derived*>(this);
    }

    const Derived& systembase() const {
        return *static_cast<const Derived*>(this);
    }
};

template <typename Derived>
void SystemBase<Derived>::concat_measures(
        std::vector< std::unique_ptr<common_measures::MeasurePolicy< Derived>> >& measures_)
{
    std::move(begin(measures_), end(measures_), std::inserter(measures, end(measures)));
}


#endif