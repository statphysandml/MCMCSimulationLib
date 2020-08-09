#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>
//#include <json.hpp>

//using json = nlohmann::json;

#include "param_helper/params.hpp"

class SystemBaseParameters : public Parameters {
public:
    using Parameters::Parameters;

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "systembase_params");
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

//    virtual void save_config(int i) = 0;

private:
    Derived& systembase() {
        return *static_cast<Derived*>(this);
    }

    const Derived& systembase() const {
        return *static_cast<const Derived*>(this);
    }
};

#endif