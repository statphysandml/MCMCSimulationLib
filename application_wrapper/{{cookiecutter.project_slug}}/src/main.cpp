#include "../include/{{ cookiecutter.project_slug }}/config.h"

#include <mcmc/mcmc_simulation/header.hpp>
#include <mcmc/mcmc_simulation/util/intervals.hpp>
#include <mcmc/modes/mode_header.hpp>
#include <mcmc/mcmc_simulation/util/random.hpp>

{%- if cookiecutter.main_template != "standard" %}
#include <command_line_support/cmdint.hpp>
{%- endif %}
{%- if cookiecutter.main_template == "cluster_support" %}
#include <cluster_support/cluster_integration.hpp>
{%- endif %}

#include "../include/{{ cookiecutter.project_slug }}/{{ cookiecutter.project_slug }}.hpp"


{%- if cookiecutter.main_template == "command_line_support" %}


struct CmdIntSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>::CmdIntSim;

    void prepare() override
    {
        auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);

        {{ cookiecutter.project_name }} system({1.0, 2.0, -1.5}, 1.0, 0.002);
        mcmc::measures::ReadableMeasure readable_measures(this->path_parameters_.get_rel_data_dir());

        auto simulation_parameters = mcmc::simulation::Simulation<{{ cookiecutter.project_name }}>::prepare_simulation_from_file(
            system, readable_measures,
            "systembase", "sigma", sigma_intervals);

        typedef mcmc::mode::EquilibriumTime EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(20, 2000, 0.1, 10, "Mean");

        typedef mcmc::mode::CorrelationTime CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(2000, 400, this->path_parameters_.get_rel_results_dir(), {"Mean"});

        typedef mcmc::mode::ExpectationValue ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
            this->path_parameters_.get_rel_results_dir(), 100000, this->path_parameters_.get_rel_results_dir(), {"Config", "Mean"}, {}, "hot", "statistical");

        // Store simulation parameters
        simulation_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
        equilibrium_time_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
        correlation_time_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
        expectation_value_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
    }
};


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    /* Example usage of mcmc::cmdint::CmdIntSim, where the prepare() method is used to specify the simulation configuration parameters.
    * 
    * The simulation parameters need to be written into a first to file by running the executable without any further arguments:
    * 
    * ./{{ cookiecutter.project_name }}
    * 
    * Afterwards, additional arguments can be used to execute the respective simulation modes, where the syntax is:
    * 
    * ./{executable} {execution_mode} {target_name} {sim_root_dir} {rel_path} {run} {eval}
    * 
    * Based on the given example, the next commands for executing the simulations are respectively:
    * 
    * ./{{ cookiecutter.project_name }} equilibrium_time {{ cookiecutter.project_name }}Simulation "./" true true true
    * 
    * for a computation of the times to equilibrium,
    * 
    * ./{{ cookiecutter.project_name }} correlation_time {{ cookiecutter.project_name }}Simulation "./" true true true
    * 
    * for a computation of the correlation times, and
    * 
    * ./{{ cookiecutter.project_name }} expectation_value {{ cookiecutter.project_name }}Simulation "./" true true true
    * 
    * for a computation of the expectation values.
    */
    CmdIntSimulation cmdint_simulation("{{ cookiecutter.project_name }}Simulation", "./", true);
    cmdint_simulation.run(argc, argv);

    /* Note that is also possible to directly call mcmc::cmdint::CmdIntSim. In this case, the default constructors are used to
    * write default configuration files to file. After adapting those to the desired settings, the simulation can be executed
    * in the same manner as above:
    */
    // mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure> cmdint_simulation("{{ cookiecutter.project_name }}Simulation", "./", true);
    // cmdint_simulation.run(argc, argv);

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
{%- elif cookiecutter.main_template == "cluster_support" %}
void prepare_simulation_parameters(const std::string &target_name, // Name of the simulation
                        const std::string &sim_root_dir = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                        const bool rel_path = true)
{
    mcmc::cmdint::PathParameters path_parameters(target_name, sim_root_dir, rel_path);

    auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);

    // Setting up the system
    {{ cookiecutter.project_name }} system({1.0, 2.0, -1.5}, 1.0, 0.002);
    mcmc::measures::ReadableMeasure readable_measures(path_parameters.get_rel_data_dir());

    auto simulation = mcmc::simulation::Simulation<{{ cookiecutter.project_name }}>::prepare_simulation_from_file(
            system, readable_measures,
            "systembase", "sigma", sigma_intervals);

    // Store simulation parameters
    simulation.write_to_file(path_parameters.get_rel_config_dir());
}


struct EquilibriumTimeSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>::CmdIntSim;

    void prepare() override
    {
        // Prepare equilibrium time simulation
        typedef mcmc::mode::EquilibriumTime EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(20, 2000, 0.1, 10, "Mean");
        equilibrium_time_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>(
                "equilibrium_time", this->path_parameters_, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::RunningMode::prep_and_exec, {});
    }
};


struct CorrelationTimeSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::CorrelationTime CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(2000, 400, this->path_parameters_.get_rel_results_dir(), "Mean");
        correlation_time_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>(
                "correlation_time", this->path_parameters_, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::RunningMode::prep_and_exec, {});
    }
};


struct ExpectationValueSimulation : mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>
{
    using mcmc::cmdint::CmdIntSim<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::ExpectationValue ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
                this->path_parameters_.get_rel_results_dir(), 100000, this->path_parameters_.get_rel_results_dir(),
                {"Config", "Mean"}, {}, "hot", "statistical");
        expectation_value_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());

        // Prepare expectation value simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>(
                "expectation_value", this->path_parameters_, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::RunningMode::prep_and_exec, {});
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
    equilibrium_time_simulation.run(argc, argv);

    CorrelationTimeSimulation correlation_time_simulation(target_name, sim_root_dir, rel_path);
    correlation_time_simulation.run(argc, argv);

    ExpectationValueSimulation expectation_value_simulation(target_name, sim_root_dir, rel_path);
    expectation_value_simulation.run(argc, argv);


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

    //[ Defining the MCMC system and further important variables

    // Name of the simulation
    const std::string target_name = "{{ cookiecutter.project_name }}Simulation";

    // Directory for storing the results
    std::string rel_results_dir = "/results/" + target_name + "/";
    // Directory for storing the simulation data
    std::string rel_data_dir = "/data/" + target_name + "/";

    // Setting up the system
    {{ cookiecutter.project_name }} system({1.0, 2.0, -1.5}, 1.0, 0.002);

    // Setting up measurement processor
    typedef mcmc::measures::ReadableMeasure ReadableMeasureProcessor;
    ReadableMeasureProcessor readable_measures(rel_data_dir);

    //]


    //[ Equilibrium time simulation

    // Setting up equilibrium time parameters
    typedef mcmc::mode::EquilibriumTime EquilibriumTimeParams;
    EquilibriumTimeParams equilibrium_time_parameters(
        20, // sample_size
        2000, // number_of_steps
        0.1, // confidence_range
        10, // confidence_window
        "Mean" // measure
    );

    // Prepare the simulation
    auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);
    auto equilibrium_time_simulation = mcmc::simulation::Simulation<
        {{ cookiecutter.project_name }}, EquilibriumTimeParams, ReadableMeasureProcessor>::generate_simulation(
            system,
            equilibrium_time_parameters,
            readable_measures,
            "systembase", // running_parameter_kind
            "sigma", // running parameter (rp)
            sigma_intervals // rp_intervals
    );
    
    // Run and evaluate the simulation
    equilibrium_time_simulation.run();
    equilibrium_time_simulation.eval(rel_results_dir);

    //]

    
    //[ Correlation time simulation

    // Relative path to the previously computed equilibrium time results
    std::string rel_equilibrium_time_results_path = "/results/" + target_name + "/";

    // Setting up correlation time parameters
    typedef mcmc::mode::CorrelationTime CorrelationTimeParams;
    CorrelationTimeParams correlation_time_parameters(
        2000, // minimum_sample_size
        400, // maximum_correlation_time
        rel_equilibrium_time_results_path, // equilibrium_time_rel_results_dir
        "Mean", // measure
        "hot" // starting_mode
    );
    
    // Prepare the simulation
    auto correlation_time_simulation = mcmc::simulation::Simulation<
        {{ cookiecutter.project_name }}, CorrelationTimeParams, ReadableMeasureProcessor>::generate_simulation(
            system,
            correlation_time_parameters,
            readable_measures,
            "systembase", // running_parameter_kind
            "sigma", // running parameter (rp)
            sigma_intervals // rp_intervals
    );

    // Run and evaluate the simulation
    correlation_time_simulation.run();
    correlation_time_simulation.eval(rel_results_dir);

    //]


    //[ Expectation Value

    // Relative path to the previously computed correlation time results
    std::string rel_correlation_time_results_path = "/results/" + target_name + "/";

    typedef mcmc::mode::ExpectationValue ExpectationValueParams;
    ExpectationValueParams expectation_value_parameters(
        rel_correlation_time_results_path, // correlation_time_rel_results_dir
        100000, //  number_of_measurements
        rel_equilibrium_time_results_path, // equilibrium_time_rel_results_dir
        {"Config", "Mean"}, // measures
         {}, // post_measures
         "hot", // starting_mode
         "statistical" // error_type
    );

    // Prepare the simulation
    auto expectation_value_simulation = mcmc::simulation::Simulation<
            {{ cookiecutter.project_name }}, ExpectationValueParams, ReadableMeasureProcessor>::generate_simulation(
            system,
            expectation_value_parameters,
            readable_measures,
            "systembase", // running_parameter_kind
            "sigma", // running parameter (rp)
            sigma_intervals // rp_intervals
    );
    
    // Run and evaluate the simulation
    expectation_value_simulation.run();
    expectation_value_simulation.eval(rel_results_dir);

    //]

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
{%- endif %}
