//
// Created by lukas on 04.12.20.
//

#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP


#include "mcmc_simulation/header.hpp"


class SystemBaseTemplate;

class SystemBaseTemplateParameters : public mcmc::simulation::SystemBaseParameters {
public:
    SystemBaseTemplateParameters(const json params):
        SystemBaseParameters(params),
        running_parameter(get_entry<double>("running_parameter"))
    {}

    std::unique_ptr<SystemBaseTemplate> generate() { return std::make_unique<SystemBaseTemplate>(*this); }

private:
    friend class SystemBaseTemplate;

    const double running_parameter;
};


class SystemBaseTemplate : public mcmc::simulation::SystemBase<SystemBaseTemplate>
{
public:
    explicit SystemBaseTemplate(const SystemBaseTemplateParameters &imsp_) : imsp(imsp_)
    {
        lattice = std::vector<double> (10, 0.0);
    }

    void update_step(uint measure_interval=1)
    {}

    void initialize(std::string starting_mode)
    {
        // Necessary
        generate_measures(imsp.measures);
    }

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

    const SystemBaseTemplateParameters &imsp;
};

#endif //MCMCSIMULATIONLIB_SYSTEMBASE_MEASURE_TEMPLATE_HPP
