//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/executer.hpp"

std::string Executer::python_scripts_path = "/home/lukas/MCMCSimulationLib/python_scripts/";
std::string Executer::executable_name = "None";
std::string Executer::cluster_mode = "locally";
std::string Executer::conda_activate_path = "/home/lukas/.miniconda3/bin/activate";
std::string Executer::virtual_env = "flowequation";

void initialize_python()
{
    Py_Initialize();
    PyRun_SimpleString(("import sys; sys.path.append('" + Executer::get_python_scripts_path() + "')").c_str());
    PyRun_SimpleString(("sys.path.append('" + Executer::get_python_scripts_path() + "/util/')").c_str());
    PyRun_SimpleString(("sys.path.append('" + Executer::get_python_scripts_path() + "/modes/')").c_str());
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
        {"equilibriate", equilibriate},
        {"plot_site_distribution", plot_site_distribution}
};

void initialize_executer_params(const std::string executable_name_, const std::string python_scripts_path_, const std::string cluster_mode_,
                                const std::string conda_activate_path_, const std::string virtual_env_)
{
    Executer::set_executable_name(executable_name_);
    Executer::set_python_scripts_path(python_scripts_path_);
    Executer::set_cluster_mode(cluster_mode_);
    Executer::set_conda_activate_path(conda_activate_path_);
    Executer::set_virtual_env(virtual_env_);
}