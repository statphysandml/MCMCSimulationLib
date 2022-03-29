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

    const std::string target_name = "ScalarTheorySimulation";

    std::string rel_data_path = "/data/" + target_name + "/";
    std::string rel_config_path = "/configs/" + target_name + "/";
    std::string rel_results_path = "/results/" + target_name + "/";
    std::string equilibrium_time_results_path = "/results/" + target_name + "/";
    std::string correlation_time_results_path = "/results/" + target_name + "/";
    auto kappa_intervals = mcmc::util::linspace(0.22, 0.3, 9);

    // Setting up system parameters
    ScalarTheoryParameters system_params(0.3, 0.02, {32, 32}, 0.14, 10, 1.0);

    mcmc::measures::ReadableMeasureParameters readable_measures(rel_data_path);

    //[ Equilibrium time

#ifdef PYTHON_BACKEND    
    typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
    EquilibriumTimeParams equilibrium_time_parameters(40, 500, 0.005, 10, "SecondMoment");

    auto simulation_params_equilibrium_time = mcmc::simulation::SimulationParameters<
            ScalarTheoryParameters, EquilibriumTimeParams>::generate_simulation(
                    system_params, equilibrium_time_parameters, readable_measures,
                    "systembase_params", "kappa", kappa_intervals);

    // Run and evaluate the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, EquilibriumTimeParams> equilibrium_time_simulation(
        simulation_params_equilibrium_time);
    equilibrium_time_simulation.run();
    equilibrium_time_simulation.eval(rel_results_path);

    //[ Correlation time

    // Setting up correlation time parameters - The correlation time will be comptued based on the "Mean" observable
    // Parameters:
    // - Minimum sample size for the computation of the correlation time,
    // - Maximum possible correlation time
    // - Number of sweeps before configurations for the computation of the correlation time are evaluated
    // - Observable to evaluate the correlation time
    typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
    CorrelationTimeParams correlation_time_parameters(1000, 400, 10000 *//*equilibrium_time_results_path*//*, {"SecondMoment"}, "cold");
    
    // Setting up the simulation
    // The simulation will execute six MCMC simulations for inverse temperatures
    // in the interval between 0.1 and 0.7 in a row.
    auto simulation_params_correlation_time = mcmc::simulation::SimulationParameters<
            ScalarTheoryParameters, CorrelationTimeParams>::generate_simulation(
                    system_params, correlation_time_parameters, readable_measures,
                    "systembase_params", "kappa", kappa_intervals);

    // Run and evaluate the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, CorrelationTimeParams> correlation_time_simulation(
        simulation_params_correlation_time);
    correlation_time_simulation.run();
    correlation_time_simulation.eval(rel_results_path);

    //]
#endif


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
    typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
    /* ExpectationValueParams expectation_value_parameters(
        correlation_time_results_path, 1000, equilibrium_time_results_path,
        {"Mean", "AbsMean", "SecondMoment", "MagenticSusceptibility", "BinderCumulant", "Action", "Config"}, {}, "hot", "statistical"); */

    /* ExpectationValueParams expectation_value_parameters(
            1, 1000, 100,
            {"Mean", "AbsMean", "SecondMoment", "Action", "Config"}, {}, "hot", "statistical"); */

    ExpectationValueParams expectation_value_parameters(
            3, 1000, 10000,
            {"Config", "Mean", "AbsMean", "SecondMoment", "Action", "AcceptanceRate", "EnergyViolation",
             "ExponentialEnergyViolation"}, {}, "hot", "statistical");

    auto simulation_params_expectation_value = mcmc::simulation::SimulationParameters<
            ScalarTheoryParameters, ExpectationValueParams>::generate_simulation(
            system_params, expectation_value_parameters, readable_measures,
            "systembase_params", "kappa", kappa_intervals);

    // Run and evaluate the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, ExpectationValueParams> expectation_value_simulation(
        simulation_params_expectation_value);
    expectation_value_simulation.run();
    expectation_value_simulation.eval(rel_results_path);

    //]

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
