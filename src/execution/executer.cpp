//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/executer.hpp"

const std::map<std::string, Executer::ExecutionMode> Executer::mode_resolver = {
        {"expectation_value", expectation_value},
        {"correlation_time", correlation_time},
        {"plot_site_distribution", plot_site_distribution}
};


void Executer::prep_exec_expectation_value(const PathParameters path_parameters)
{
    ExpectationValueParameters expectation_value_parameters(json {});
    expectation_value_parameters.write_to_file(path_parameters.get_rel_config_path());
}