#include "../include/{{ cookiecutter.project_slug }}/config.h"

#include <mcmc_simulation/header.hpp>
#include <execution/executer.hpp>

#include "../include/{{ cookiecutter.project_slug }}/simulation_header.hpp"


void custom_main();

int main(int argc, char **argv) {
    param_helper::fs::prfs::set_relative_path_to_project_root_dir("../");

    std::cout << "Project root dir" << param_helper::fs::prfs::path_to_executable() << std::endl;

    // Initialization - Only needed for GPU and CPU runs
    mcmc::execution::initialize_executer_params(PROJECT_NAME, CLUSTER_MODE);

#ifdef RUN_WITH_PYTHON_BACKEND
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

    // Finalization
#ifdef RUN_WITH_PYTHON_BACKEND
    mcmc::execution::finalize_python();
#endif
    return 0;
}

void custom_main() {
    
    // Setting up direction parameters
    std::string target_name = "IsingModelSimulation";
    std::string rel_data_path = "/data/" + target_name + "/";
    std::string rel_config_path = "/configs/" + target_name + "/";
    std::string correlation_time_results_path = "/results/" + target_name + "/";

    // Setting up system parameters
    SystemParameters system_params(0.4, 1.0, 0.0, {4, 4});
    /* // Alternativ:
    SystemParameters system_params(
            json {{ '{{"beta",  0.4},
                  {"J", 1.0},
                  {"h", 0.0},
                  {"dimensions", std::vector<int> {4, 4}}' }}
            }); */
    
    //[ Correlation time

    // Setting up correlation time parameters - The correlation time will be comptued based on the "Mean" observable
    // Parameters:
    // - Minimum sample size for the computation of the correlation time,
    // - Maximum possible correlation time
    // - Number of sweeps before configurations for the computation of the correlation time are evaluated
    // - Observable to evaluate the correlation time
    typedef mcmc::execution::CorrelationTimeParameters CorrelationTimeParams;
    CorrelationTimeParams correlation_time_execution_parameters(1000, 400, 1000, {"Mean"});
    
    // Setting up the simulation
    // The simulation will execute six MCMC simulations for inverse temperatures
    // in the interval between 0.1 and 0.7 in a row.
    auto simulation_params_correlation_time = mcmc::simulation::SimulationParameters<
            SystemParameters, CorrelationTimeParams>::generate_simulation(
                    system_params, correlation_time_execution_parameters, rel_data_path,
                    "systembase_params", "beta", 0.1, 0.7, 7);

    // Store execution and simulation parameters as config files for a possible later simulation.
    // The correlation time parameters are stored also automatically since they are part of simulation parameters
    simulation_params_correlation_time.write_to_file(rel_config_path);

    // Run the simulation
    mcmc::execution::execute<SystemParameters>(mcmc::execution::CorrelationTimeParameters::name(), target_name);

    //]

    //[ Expectation Value

    // Setting up execution parameters
    // Parameters:
    // - Correlation time - The simulation will use the evaluated correlation times from the previous
    // computation. Alternatively, this could also be an integer number, if the previous simulation has
    // not been done.
    // - Number of measurements
    // - Number of sweeps before the first measurement
    // - Observables that are computed within the simulation
    // - Observables that are computed afterwards in Python
    // The observables: "AbsMean", "SecondMoment", "Mean"  are computed within C++ during the
    // simulation. The "Energy" is computed in Python afterwards based on the configurations ("Config") 
    typedef mcmc::execution::ExpectationValueParameters ExpectationValueParams;
    ExpectationValueParams execution_parameters(correlation_time_results_path, 20000, 1000, {"AbsMean", "SecondMoment", "Mean", "Config"}, {"Energy"});

    auto simulation_params_expectation_value = mcmc::simulation::SimulationParameters<
            SystemParameters, ExpectationValueParams>::generate_simulation(
            system_params, execution_parameters, rel_data_path,
            "systembase_params", "beta", 0.1, 0.7, 7);

    // Store simulation parameters
    simulation_params_expectation_value.write_to_file("/configs/" + target_name + "/");

    // Run the simulation
    mcmc::execution::execute<SystemParameters>(mcmc::execution::ExpectationValueParameters::name(), target_name);

    //]
    
}
