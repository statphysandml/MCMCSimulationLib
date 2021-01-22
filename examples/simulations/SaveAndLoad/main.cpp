#ifndef MAIN_CPP
#define MAIN_CPP

#include "config.h"
#include "simulation_header.hpp"

#endif

void custom_main();

int main(int argc, char **argv) {
    param_helper::fs::prfs::set_relative_path_to_project_root_dir("../");

    // Initialization - Only needed for GPU and CPU runs
    mcmc::execution::initialize_executer_params(PROJECT_NAME, CLUSTER_MODE);

#ifdef PYTHON
    mcmc::execution::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    // The function of the first if-condition is only called when an actual simulation takes place based on arguments
    // (or for the generation of default parameters) based on a program that uses ./Main with arguments (from cpu/gpu/locally)
    if(argc > 1)
    {
        mcmc::execution::run_from_file<typename from_file_simulation::SystemBaseParams>(argc, argv);
    }
    else
        // Helpful for a preparation of the simulation or immediate execution (on cpu/gpu/locally, testing/running directly)
        custom_main();

#ifdef PYTHON
    mcmc::execution::finalize_python();
#endif
    return 0;
}


/* Save simulating parameters and load simulation from file - On the example of the Execution mode: CorrelationTime */

void custom_main() {
    // Setting up system parameters
    SystemBaseTemplateParameters system_params(json {{"running_parameter", 0.0}});


    // Setting up correlation time parameters
    typedef mcmc::execution::CorrelationTimeParameters CorrelationTimeParams;
    CorrelationTimeParams correlation_time_execution_parameters(10, 100, 100);


    // Setting up simulation parameters
    std::string target_name = "SaveAndLoad";
    std::string rel_data_path = "/data/" + target_name + "/";
    auto simulation_params = mcmc::simulation::SimulationParameters< SystemBaseTemplateParameters , CorrelationTimeParams >::generate_simulation(
            system_params, correlation_time_execution_parameters, rel_data_path,
            "systembase_params", "running_parameter", 0.0, 1.0, 5);


    // Store execution and simulation parameters as config files for a possible later simulation - In this case,
    // the expectation values are stored also automatically since they are part of simulation parameters
    std::string rel_sim_params_path = "/configs/" + target_name + "/";
    simulation_params.write_to_file(rel_sim_params_path);


    std::cout << "\nAll necessary parameters for the simulation have been written to rel_config_path" << std::endl;


    // Load simulation parameters from file - Note: It is important that the template parameters coincide for storing and loading
    auto from_file_simulation_params = mcmc::simulation::SimulationParameters< SystemBaseTemplateParameters , CorrelationTimeParams >::generate_simulation_from_file(
            rel_sim_params_path);

    std::cout << "\nFrom file simulation params: \n" << from_file_simulation_params << std::endl;

    // Setting up and running the actual simulation
    mcmc::simulation::Simulation< SystemBaseTemplateParameters, CorrelationTimeParams > from_file_simulation(from_file_simulation_params);
    from_file_simulation.run();
}
