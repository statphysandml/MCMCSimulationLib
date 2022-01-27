#ifndef MAIN_CPP
#define MAIN_CPP

#include "../include/simulation_and_execution/config.h"

#include <mcmc_simulation/header.hpp>
#include <execution/executer.hpp>

#include "../include/simulation_and_execution/simulation_header.hpp"

#endif

void custom_main();

int main(int argc, char **argv) {
    param_helper::proj::set_relative_path_to_project_root_dir("../");

    // Initialization - Only needed for GPU and CPU runs
    mcmc::execution::initialize_executer_params(PROJECT_NAME, CLUSTER_MODE);

#ifdef PYTHON_BACKEND
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

#ifdef PYTHON_BACKEND
    mcmc::execution::finalize_python();
#endif
    return 0;
}

/* A simple simulation - On the example of the Execution mode: ExpectationValue
 * Further, the different ways to execute code are explained.
 */

void custom_main()
{

    // Setting up system parameters
    SystemBaseTemplateParameters system_params(json {{"running_parameter", 0.0}});


    // Setting up execution parameters - "measures" can only be defined here if SystemBaseTemplateParameters is used
    typedef mcmc::execution::ExpectationValueParameters ExpectationValueParams;
    ExpectationValueParams execution_parameters(100, 10000, 100, {"Mean"}, {});


    // Setting up simulation parameters
    std::string target_name = "SimpleSimulation";
    std::string rel_data_path = "/data/" + target_name + "/";
    auto simulation_params = mcmc::simulation::SimulationParameters< SystemBaseTemplateParameters , ExpectationValueParams >::generate_simulation(
            system_params, execution_parameters, rel_data_path,
            "systembase_params", "running_parameter", 0.0, 1.0, 5);

    // Setting up and running the actual simulation
    mcmc::simulation::Simulation< SystemBaseTemplateParameters, ExpectationValueParams > simulation(simulation_params);
    simulation.run();

    // Alternatively, you can perform the same with executer,

    // Store the simulation parameters - Only necessary if one wants to run the simulation again or on a CPU cluster, for example.
    std::string rel_sim_params_path = "/configs/" + target_name + "/";
    simulation_params.write_to_file(rel_sim_params_path);

    // After storing, there are the following ways to run the simulation:

    // a)
    // Load the simulation parameters and run the simulation as before
    // -> See save_and_load for an example.

    // b)
    // With the help of the executer. In this case, the expectation values are also computed in python if you have
    // python integrated - According to the ExpectationValue Mode
    std::cout << "\n\nSecond run of the simulation\n\n" << std::endl;
    mcmc::execution::execute< SystemBaseTemplateParameters > (ExpectationValueParams::name(), target_name);

    // c)
    // By running:
    //     ./SimulateAndExecute expectation_value SimpleSimulation
    std::cout << "\n\nYou can rerun the simulation by running './SimulateAndExecute expectation_value SimpleSimulation\n\n" << std::endl;
    // in the terminal. This only works if and only if you keep the respective if-function in the main function AND the
    // provided systembase template parameter coincides with the one in your config files. With the command exactly
    // the same as in b) will happen, with the exception that the systembase template parameter is passed in the main
    // function of the project/simulation.

    // d)
    // By changing further parameters of the execute command, you can submit a job to run the code on a cluster
    // (execute_code=true) or only prepare the respective bash script in the cpu_cluster_runs/ directory of the
    // project (execute_code=false). On the cluster, the simulation is executed based on the code in c). Accordingly, it is again
    // important that the restrictions to the correct system base parameters in c) hold. This behaviour can be changed by
    // changing the main function. To have higher flexibility, the execute function has a vector of strings as a last argument. This
    // allows passing additional parameters via the bash script to the main function of the project/simulation (of this file) when
    // the code is actually executed on the cluster.
    std::string sim_root_dir = "./";
    bool rel_path = true;
    bool execute_code = true;  // Change to false to only generate the necessary bash_script for the exeuction on a cluster.
    // The bash_script is generated in the cpu_cluster_runs/ directory. In dependence on the cluster_mode variable,
    // the execute function either submits the job to the cluster or executes the code locally on your machine. The (terminal)
    // ouput of the simulation is in both cases copied to the cpu_cluster_runs/ directory.
    std::vector<std::string> additional_args = {};
    mcmc::execution::execute< SystemBaseTemplateParameters > (
            ExpectationValueParams::name(), target_name, sim_root_dir, rel_path,
            mcmc::execution::Executer::on_cpu_cluster, execute_code, additional_args);

    // Additional remarks: By passing the cluster_mode with -DCLUSTER_MODE="local" or -DCLUSTER_MODE="on_cluster"
    // to cmake, you can switch between a "testing" mode, where the job is executed locally ("local") and an actual submission to
    // the cluster ("on_cluster"). For example, execute in the release directory:
    //     cmake ../cmake/ -DCMAKE_BUILD_TYPE=Release -DCLUSTER_MODE=on_cluster
    //     make -j4
}

