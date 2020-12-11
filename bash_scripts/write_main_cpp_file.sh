cat >"${src_path}/main.cpp" <<EOL
#ifndef MAIN_CPP
#define MAIN_CPP

EOL
if [ "$project_type" = "project" ]; then
cat >>"${src_path}/main.cpp" <<EOL
#include "../include/config.h"
#include "../include/simulation_header.hpp"
EOL
else
cat >>"${src_path}/main.cpp" <<EOL
#include "config.h"
#include "simulation_header.hpp"
EOL
fi
cat >>"${src_path}/main.cpp" <<EOL

#endif

void custom_main();

int main(int argc, char **argv) {
    param_helper::fs::prfs::set_relative_path_to_project_root_dir("/../");
    // mcmc::execution::initialize_executer_params(PROJECT_NAME, PYTHON_SCRIPTS_PATH, CLUSTER_MODE, CONDA_ACTIVATE_PATH, VIRTUAL_ENV);

#ifdef PYTHON
    mcmc::execution::initialize_python();
#endif

    // A function of one of the first three if conditions is only called when an actual simulation takes place
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
}

void custom_main()
{
    // Setting up system parameters
    SystemBaseTemplateParameters system_params(json {{"running_parameter", 0.0}});


    // Setting up execution parameters - "measures" can only be defined here if SystemBaseTemplateParameters is used
    typedef mcmc::execution::ExpectationValueParameters ExpectationValueParams;
    ExpectationValueParams execution_parameters(100, 10000, 100, {}, {});


    // Setting up simulation parameters
    std::string target_name = "SimpleSimulation";
    std::string rel_data_path = "/data/" + target_name + "/";
    auto simulation_params = mcmc::simulation::SimulationParameters< SystemBaseTemplateParameters , ExpectationValueParams >::generate_simulation(
            system_params, execution_parameters, rel_data_path,
            "systembase_params", "running_parameter", 0.0, 1.0, 5);


    // Setting up and running the actual simulation
    mcmc::simulation::Simulation< SystemBaseTemplateParameters, ExpectationValueParams > simulation(simulation_params);
    simulation.run();

    // Store the simulation parameters
    std::string rel_sim_params_path = "/configs/" + target_name + "/";
    simulation_params.write_to_file(rel_sim_params_path);
    std::cout << "\nYou can rerun the simulation by rerunning with './$project_name expectation_value SimpleSimulation" << std::endl;
}
EOL
