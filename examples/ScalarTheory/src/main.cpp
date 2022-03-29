#include "../include/scalar_theory/config.h"

#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/intervals.hpp>
#include <modes/mode_header.hpp>
#include <mcmc_simulation/util/random.hpp>

#include "../include/scalar_theory/scalar_theory.hpp"


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    //[ Defining the considiered system and further important variables

    // Name of the simulation
    const std::string target_name = "ScalarTheorySimulation";

    // Directory for storing the results
    std::string rel_results_path = "/results/" + target_name + "/";
    // Directory for storing the simulation data
    std::string rel_data_path = "/data/" + target_name + "/";

    // Setting up system parameters
    ScalarTheoryParameters system_params(0.3, 0.02, {32, 32}, 0.14, 10, 1.0);

    // Setting up measurement processing parameters
    mcmc::measures::ReadableMeasureParameters readable_measures(rel_data_path);

    //]

    //[ Equilibrium time simulation

    // Setting up equilibrium time parameters
    typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
    EquilibriumTimeParams equilibrium_time_parameters(
        40, // sample_size
        500, // number_of_steps
        0.005, // confidence_range
        10, // confidence_window
        "SecondMoment" // measure
    );

    // Setting up simulation parameters
    auto kappa_intervals = mcmc::util::linspace(0.22, 0.3, 9);
    auto simulation_params_equilibrium_time = mcmc::simulation::SimulationParameters<
        ScalarTheoryParameters, EquilibriumTimeParams>::generate_simulation(
            system_params,
            equilibrium_time_parameters,
            readable_measures,
            "systembase_params", // running_parameter_kind
            "kappa", // running parameter (rp)
            kappa_intervals // rp_intervals
    );

    // Prepare the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, EquilibriumTimeParams> equilibrium_time_simulation(
        simulation_params_equilibrium_time);
    
    // Run and evaluate the simulation
    equilibrium_time_simulation.run();
    equilibrium_time_simulation.eval(rel_results_path);

    //]
    
    //[ Correlation time simulation

    // Relative path to the previously computed equilibrium time results
    std::string rel_equilibrium_time_results_path = "/data/" + target_name + "/";

    // Setting up correlation time parameters
    typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
    CorrelationTimeParams correlation_time_parameters(
        1000, // minimum_sample_size
        400, // maximum_correlation_time
        rel_equilibrium_time_results_path, // equilibrium_time_rel_results_path
        "SecondMoment", // measure
        "cold" // starting_mode
    );
    
    // Setting up the simulation parameters preparing the actual simulation with different values of kappa
    auto simulation_params_correlation_time = mcmc::simulation::SimulationParameters<
        ScalarTheoryParameters, CorrelationTimeParams>::generate_simulation(
            system_params,
            correlation_time_parameters,
            readable_measures,
            "systembase_params", // running_parameter_kind
            "kappa", // running parameter (rp)
            kappa_intervals // rp_intervals
    );

    // Prepare the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, CorrelationTimeParams> correlation_time_simulation(
        simulation_params_correlation_time);

    // Run and evaluate the simulation
    correlation_time_simulation.run();
    correlation_time_simulation.eval(rel_results_path);

    //]

    //[ Expectation Value

    // Relative path to the previously computed correlation time results
    std::string rel_correlation_time_results_path = "/data/" + target_name + "/";

    typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
    ExpectationValueParams expectation_value_parameters(
        rel_correlation_time_results_path, // correlation_time_rel_results_path
        1000, //  number_of_measurements
        rel_equilibrium_time_results_path, // equilibrium_time_rel_results_path
        {"Config", "Mean", "AbsMean", "SecondMoment", "Action", "AcceptanceRate", "EnergyViolation",
         "ExponentialEnergyViolation"}, // measures
         {}, // post_measures
         "cold", // starting_mode
         "statistical" // error_type
    );

    // Setting up the simulation parameters
    auto simulation_params_expectation_value = mcmc::simulation::SimulationParameters<
            ScalarTheoryParameters, ExpectationValueParams>::generate_simulation(
            system_params,
            expectation_value_parameters,
            readable_measures,
            "systembase_params", // running_parameter_kind
            "kappa", // running parameter (rp)
            kappa_intervals // rp_intervals
    );

    // Prepare the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, ExpectationValueParams> expectation_value_simulation(
        simulation_params_expectation_value);
    
    // Run and evaluate the simulation
    expectation_value_simulation.run();
    expectation_value_simulation.eval(rel_results_path);

    //]

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
