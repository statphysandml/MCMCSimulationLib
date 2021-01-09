//
// Created by lukas on 04.12.20.
//

#ifndef MCMCSIMULATIONLIB_STORING_AND_LOADING_HPP
#define MCMCSIMULATIONLIB_STORING_AND_LOADING_HPP

#include "mcmc_simulation/header.hpp"
#include "execution/modes/correlation_time.hpp"

// For systembases with the possible usage of predefined common measures
#include "../../templates/systembase_template.hpp"

// For systembases with custom measures
#include "../../templates/plain_systembase_template.hpp"

/* Save simulating parameters and load simulation from file - Execution mode: CorrelationTime */

void save_and_load() {
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
    mcmc::simulation::Simulation< SystemBaseTemplateParameters, CorrelationTimeParams > from_file_simulation(simulation_params);
    from_file_simulation.run();
}

#endif //MCMCSIMULATIONLIB_STORING_AND_LOADING_HPP
