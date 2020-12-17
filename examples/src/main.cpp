//
// Created by lukas on 04.12.20.
//

#include "../include/config.h"

#include "mcmc_simulation/header.hpp"

#include "execution/modes/expectation_value.hpp"
#include "execution/modes/correlation_time.hpp"
#include "execution/executer.hpp"

// For systembases with the possible usage of predefined common measures
#include "../../templates/systembase_template.hpp"

// For systembases with custom measures
#include "../../templates/plain_systembase_template.hpp"


typedef SystemBaseTemplateParameters SystemBaseParams;

void custom_main();

int main(int argc, char **argv) {
    param_helper::fs::prfs::set_relative_path_to_project_root_dir("/../");

    // Initialization
    // mcmc::execution::initialize_executer_params(PROJECT_NAME, CLUSTER_MODE, CONDA_ACTIVATE_PATH, VIRTUAL_ENV, PYTHON_SCRIPTS_PATH);

#ifdef PYTHON
    mcmc::execution::initialize_python();
#endif

    // A function of one of the first three if conditions is only called when an actual simulation takes place
    // (or for the generation of default parameters) based on a program that uses ./Main with arguments (from cpu/gpu/locally)
    if(argc > 1)
    {
        mcmc::execution::run_from_file<SystemBaseParams>(argc, argv);
    }
    else
        // Helpful for a preparation of the simulation or immediate execution (on cpu/gpu/locally, testing/running directly)
        custom_main();

    // Finalization
#ifdef PYTHON
    mcmc::execution::finalize_python();
#endif
    return 0;

}

#include "../include/simple_simulation.hpp"
#include "../include/save_and_load.hpp"
#include "../include/ising_model.hpp"

void custom_main() {
    /* simple_simulation();
    save_and_load();
    ising_model_simulation(); */
    ising_model_simulation_with_correlation_time();

    /* // Setting up system parameters - This does also work with SystemBaseMeasureTemplateParameters
    SystemBaseParams system_params(json {{"running_parameter", 0.0}});


    // Setting up execution parameters - "measures" can only be defined here if SystemBaseMeasuresTemplateParameters is used
    typedef mcmc::execution::ExpectationValueParameters ExpectationValueParams;
    ExpectationValueParams execution_parameters(100, 10000, 100, {}, {});


    // Setting up simulation parameters
    std::string target_name = "HelloWorldSimulation";
    std::string rel_data_path = "/data/" + target_name + "/";
    auto simulation_params = mcmc::simulation::SimulationParameters< SystemBaseParams , ExpectationValueParams >::generate_simulation(
            system_params, execution_parameters, rel_data_path,
            "systembase_params", "running_parameter", 0.0, 1.0, 5);

    // Store execution and simulation parameters as config files for a possible later simulation - In this case,
    // the expectation values are stored also automatically since they are part of simulation parameters
    std::string rel_sim_params_path = "/configs/" + target_name + "/";
    simulation_params.write_to_file(rel_sim_params_path);

    // Using execute to execute the code directly
    mcmc::execution::execute<SystemBaseParams>(
            ExpectationValueParams::name(), // = "expectation_value"
            target_name, // = "HelloWorldSimulation"
            "/./", // default
            true, // default
            mcmc::execution::Executer::local, // mcmc::execution::Executer::local can be used to test the code first locally
            true); // In this case, the bash script can be started with: qsub ... or nice -n 17 bash ...
    */
}