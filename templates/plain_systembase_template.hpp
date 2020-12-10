//
// Created by lukas on 03.12.20.
//

#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_TEMPLATE_CUSTOM_MEASURES_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_TEMPLATE_CUSTOM_MEASURES_HPP


#include "mcmc_simulation/header.hpp"


class PlainSystemBaseTemplate;

class PlainSystemBaseTemplateParameters : public mcmc::simulation::PlainSystemBaseParameters {
public:
    PlainSystemBaseTemplateParameters(const json params):
        PlainSystemBaseParameters(params),
        running_parameter(get_entry<double>("running_parameter"))
    {}

    std::unique_ptr<PlainSystemBaseTemplate> generate() { return std::make_unique<PlainSystemBaseTemplate>(*this); }

    /* static const std::string name() // Optional
    {
        return "ising_model_system_params";
    }

    void write_to_file(const std::string& root_dir) { // Optional - Needs to be defined if name() function is defined
        Parameters::write_to_file(root_dir, name());
    } */

private:
    friend class PlainSystemBaseTemplate;

    const double running_parameter;
};


class PlainSystemBaseTemplate : public mcmc::simulation::PlainSystemBase<PlainSystemBaseTemplate>
{
public:
    explicit PlainSystemBaseTemplate(const PlainSystemBaseTemplateParameters &imsp_) : imsp(imsp_)
    {}

    void update_step(uint measure_interval=1)
    {}

    void initialize(std::string starting_mode)
    {}

    auto get_size() const
    {
        return lattice.size();
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

    // Optional - For standard measurements, the already implemented method can be used
    std::vector<std::string> perform_measure()
    {
        std::vector<std::string> results;
        results.push_back(std::to_string(0.0));
        return results;
    }

    // Optional - For standard measurements, the already implemented method can be used
    std::vector<std::string> get_measure_names()
    {
        std::vector<std::string> measure_names;
        measure_names.emplace_back("default_measure");
        return measure_names;
    }


private:
    // Can be replaced by any other representation
    std::vector<double> lattice;

    const PlainSystemBaseTemplateParameters &imsp;
};


#endif //MCMCSIMULATIONLIB_SYSTEMBASE_TEMPLATE_CUSTOM_MEASURES_HPP
