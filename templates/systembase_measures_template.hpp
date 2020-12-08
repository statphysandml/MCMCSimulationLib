//
// Created by lukas on 04.12.20.
//

#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP


#include "../include/mcmc_simulation/header.hpp"


class SystemBaseMeasuresTemplate;

class SystemBaseMeasuresTemplateParameters : public mcmc::simulation::SystemBaseMeasuresParameters {
public:
    SystemBaseMeasuresTemplateParameters(const json params):
        SystemBaseMeasuresParameters(params),
        running_parameter(get_entry<double>("running_parameter"))
    {}

    std::unique_ptr<SystemBaseMeasuresTemplate> generate() { return std::make_unique<SystemBaseMeasuresTemplate>(*this); }

    /* static const std::string name() // Optional
    {
        return "ising_model_system_params";
    }

    void write_to_file(const std::string& root_dir) { // Optional - Needs to be defined if name() function is defined
        Parameters::write_to_file(root_dir, name());
    } */

private:
    friend class SystemBaseMeasuresTemplate;

    const double running_parameter;
};


class SystemBaseMeasuresTemplate : public mcmc::simulation::SystemBaseMeasures<SystemBaseMeasuresTemplate>
{
public:
    explicit SystemBaseMeasuresTemplate(const SystemBaseMeasuresTemplateParameters &imsp_) : imsp(imsp_)
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


private:
    // Can be replaced by any other representation
    std::vector<double> lattice;

    const SystemBaseMeasuresTemplateParameters &imsp;
};

#endif //MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP
