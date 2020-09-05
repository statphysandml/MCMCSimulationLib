//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/executer.hpp"

std::string get_python_scripts_path()
{
    return "/home/lukas/MCMCSimulationLib/python_scripts/";
}

void initialize_python()
{
    Py_Initialize();
    // PyRun_SimpleString(("sys.path.append('" + get_python_scripts_path() + "')").c_str());
    // PyRun_SimpleString(("sys.path.append('" + get_python_scripts_path() + "/modes/')").c_str());
    /* PyRun_SimpleString("sys.path.append( os.path.dirname(os.getcwd()) + '/python_scripts/')");
    PyRun_SimpleString("sys.path.append( os.path.dirname(os.getcwd()) + '/python_scripts/plotting_environment/')");
    PyRun_SimpleString("print('Running python in ' + os.path.dirname(os.getcwd()) + '/python_scripts/')"); */

}

void finalize_python()
{
    Py_Finalize();
}


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