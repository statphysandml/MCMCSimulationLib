#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>
//#include <json.hpp>

//using json = nlohmann::json;

#include "param_helper/params.hpp"
#include "measure_policy.hpp"


class SystemBaseParameters : public Parameters {
public:
    using Parameters::Parameters;

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, param_file_name());
    }

    static const std::string param_file_name()
    {
        return "systembase_params";
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

    std::vector<std::string> measure()
    {
        return systembase().perform_measure();
    }

    std::vector<std::string> measure_names()
    {
        return systembase().get_measure_names();
    }

    /*auto& get_system()
    {
        return systembase().get_system();
    }*/

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

    std::vector<std::string> perform_measure()
    {
        std::vector<std::string> results;
        for(auto const& element: measures) {
            results.push_back(element->measure(systembase()));
        }
        return results;
    }

    std::vector<std::string> get_measure_names()
    {
        std::vector<std::string> results;
        for(auto const& element: measures) {
            results.push_back(element->name());
        }
        return results;
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