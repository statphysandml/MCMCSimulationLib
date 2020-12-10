//
// Created by lukas on 04.12.20.
//

#ifndef MCMCSIMULATIONLIB_HELLO_WORLD_SIMULATION_HPP
#define MCMCSIMULATIONLIB_HELLO_WORLD_SIMULATION_HPP


#include "mcmc_simulation/header.hpp"
#include "execution/modes/expectation_value.hpp"

// For systembases with the possible usage of predefined common measures
#include "../../templates/systembase_template.hpp"

// For systembases with custom measures
#include "../../templates/plain_systembase_template.hpp"

/* A simple simulation - Execution mode: ExpectationValue */

void simple_simulation() {

    // Setting up system parameters - This does also work with SystemBaseMeasureTemplateParameters
    SystemBaseTemplateParameters system_params(json {{"running_parameter", 0.0}});


    // Setting up execution parameters - "measures" can only be defined here if SystemBaseMeasuresTemplateParameters is used
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
}

#endif //MCMCSIMULATIONLIB_HELLO_WORLD_SIMULATION_HPP
