#include "../include/{{ cookiecutter.project_slug }}/config.h"

#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/intervals.hpp>
#include <modes/mode_header.hpp>
#include <mcmc_simulation/util/random.hpp>

#include "../include/{{ cookiecutter.project_slug }}/{{ cookiecutter.project_slug }}.hpp"


{%- if cookiecutter.main_template == "command_line_support" %}
struct CmdIntSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        auto sigma_intervals = mcmc::util::linspace(0.5, 1.5, 9);

        {{ cookiecutter.project_name }}Parameters system_params({1.0, 2.0, -1.5}, 1.0, 0.01);
        mcmc::measures::ReadableMeasureParameters readable_measures(this->path_parameters.get_rel_data_path());

        auto simulation_parameters = mcmc::simulation::SimulationParameters<{{ cookiecutter.project_name }}Parameters>::prepare_simulation_from_file(
            system_params, readable_measures,
            "systembase_params", "sigma", sigma_intervals);

        typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(100, 1000, 0.1, 10, "Mean");

        typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(1000, 400, this->path_parameters.get_rel_results_path(), {"Mean"});

        typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
            this->path_parameters.get_rel_results_path(), 20000, this->path_parameters.get_rel_results_path(), {"Config", "Mean"}, {}, "hot", "statistical");

        // Store simulation parameters
        simulation_parameters.write_to_file(this->path_parameters.get_rel_config_path());
        equilibrium_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());
        correlation_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());
        expectation_value_parameters.write_to_file(this->path_parameters.get_rel_config_path());
    }
};


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    CmdIntSimulation cmdint_simulation("{{ cookiecutter.project_name }}Simulation", "./", true);
    cmdint_simulation.main(argc, argv);

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
{%- elif cookiecutter.main_template == "cluster_support" %}
void prepare_simulation_parameters(const std::string target_name, // Name of the simulation
                        const std::string sim_root_dir = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                        const bool rel_path = true)
{
    mcmc::cmdint::PathParameters path_parameters(target_name, sim_root_dir, rel_path);

    auto sigma_intervals = mcmc::util::linspace(0.5, 1.5, 9);

    // Setting up system parameters
    {{ cookiecutter.project_name }}Parameters system_params({1.0, 2.0, -1.5}, 1.0, 0.01);
    mcmc::measures::ReadableMeasureParameters readable_measures(path_parameters.get_rel_data_path());

    auto simulation_parameters = mcmc::simulation::SimulationParameters<{{ cookiecutter.project_name }}Parameters>::prepare_simulation_from_file(
            system_params, readable_measures,
            "systembase_params", "sigma", sigma_intervals);

    // Store simulation parameters
    simulation_parameters.write_to_file(path_parameters.get_rel_config_path());
}


struct EquilibriumTimeSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare equilibrium time simulation
        typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(20, 100, 0.05, 10, "Mean");
        equilibrium_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>(
                "equilibrium_time", this->path_parameters, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
    }
};


struct CorrelationTimeSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(1000, 400, this->path_parameters.get_rel_results_path(), "Mean");
        correlation_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>(
                "correlation_time", this->path_parameters, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
    }
};


struct ExpectationValueSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
                this->path_parameters.get_rel_results_path(), 200, this->path_parameters.get_rel_results_path(),
                {"Config", "Mean"}, {}, "hot", "statistical");
        expectation_value_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare expectation value simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}Parameters, mcmc::measures::ReadableMeasureParameters>(
                "expectation_value", this->path_parameters, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
    }
};


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif
    mcmc::cluster::initialize_cluster_params(PROJECT_NAME, CLUSTER_MODE);

    const std::string target_name = "{{ cookiecutter.project_name }}Simulation";
    const std::string sim_root_dir = "./";
    const bool rel_path = true;

    prepare_simulation_parameters(target_name, sim_root_dir, rel_path);

    EquilibriumTimeSimulation equilibrium_time_simulation(target_name, sim_root_dir, rel_path);
    equilibrium_time_simulation.main(argc, argv);

    CorrelationTimeSimulation correlation_time_simulation(target_name, sim_root_dir, rel_path);
    correlation_time_simulation.main(argc, argv);

    ExpectationValueSimulation expectation_value_simulation(target_name, sim_root_dir, rel_path);
    expectation_value_simulation.main(argc, argv);


    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
{%- else %}
int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    const std::string target_name = "{{ cookiecutter.project_name }}Simulation";

    std::string rel_data_path = "/data/" + target_name + "/";
    std::string rel_config_path = "/configs/" + target_name + "/";
    std::string rel_results_path = "/results/" + target_name + "/";
    std::string equilibrium_time_results_path = "/results/" + target_name + "/";
    std::string correlation_time_results_path = "/results/" + target_name + "/";
    auto sigma_intervals = mcmc::util::linspace(0.5, 1.5, 9);

    // Setting up system parameters
    {{ cookiecutter.project_name }}Parameters system_params({1.0, 2.0, -1.5}, 1.0, 0.01);

    mcmc::measures::ReadableMeasureParameters readable_measures(rel_data_path);

    //[ Equilibrium time
    
    /* typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
    EquilibriumTimeParams equilibrium_time_parameters(40, 500, 0.005, 10, "SecondMoment");

    auto simulation_params_equilibrium_time = mcmc::simulation::SimulationParameters<
            ScalarTheoryParameters, EquilibriumTimeParams>::generate_simulation(
                    system_params, equilibrium_time_parameters, readable_measures,
                    "systembase_params", "kappa", kappa_intervals);

    // Run and evaluate the simulation
    mcmc::simulation::Simulation<ScalarTheoryParameters, EquilibriumTimeParams> equilibrium_time_simulation(
        simulation_params_equilibrium_time);
    equilibrium_time_simulation.run();
    equilibrium_time_simulation.eval(rel_results_path); */

    //[ Correlation time

    // Setting up correlation time parameters - The correlation time will be comptued based on the "Mean" observable
    // Parameters:
    // - Minimum sample size for the computation of the correlation time,
    // - Maximum possible correlation time
    // - Number of sweeps before configurations for the computation of the correlation time are evaluated
    // - Observable to evaluate the correlation time
    /* typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
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
    correlation_time_simulation.eval(rel_results_path); */

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
    typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
    /* ExpectationValueParams expectation_value_parameters(
        correlation_time_results_path, 1000, equilibrium_time_results_path,
        {"Mean", "AbsMean", "SecondMoment", "MagenticSusceptibility", "BinderCumulant", "Action", "Config"}, {}, "hot", "statistical"); */

    /* ExpectationValueParams expectation_value_parameters(
            1, 1000, 100,
            {"Mean", "AbsMean", "SecondMoment", "Action", "Config"}, {}, "hot", "statistical"); */

    ExpectationValueParams expectation_value_parameters(
            3, 1000, 1000,
            {"Config", "Mean"}, {}, "hot", "statistical");

    auto simulation_params_expectation_value = mcmc::simulation::SimulationParameters<
            {{ cookiecutter.project_name }}Parameters, ExpectationValueParams>::generate_simulation(
            system_params, expectation_value_parameters, readable_measures,
            "systembase_params", "sigma", sigma_intervals);

    // Run and evaluate the simulation
    mcmc::simulation::Simulation<{{ cookiecutter.project_name }}Parameters, ExpectationValueParams> expectation_value_simulation(
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
{%- endif %}
