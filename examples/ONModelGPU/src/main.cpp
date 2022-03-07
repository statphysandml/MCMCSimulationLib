#include "../include/onmodel_gpu/config.h"

#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/intervals.hpp>
#include <modes/mode_header.hpp>
#include <mcmc_simulation/util/random.hpp>

#include <command_line_support/cmdint.hpp>
#include <cluster_support/cluster_integration.hpp>

#include "../include/onmodel_gpu/onmodel_gpu.hpp"

template<uint N>
void prepare_simulation_parameters(const std::string target_name, // Name of the simulation
                        const std::string sim_root_dir = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                        const bool rel_path = true)
{
    mcmc::cmdint::PathParameters path_parameters(target_name, sim_root_dir, rel_path);

    auto kappa_intervals = mcmc::util::linspace(0.22, 0.3, 9);

    // Setting up system parameters
    ONModelGPUParameters<N> system_params(0.3, 0.02, {4, 4}, 0.01);
    mcmc::measures::ReadableMeasureParameters readable_measures(path_parameters.get_rel_data_path());

    auto simulation_parameters = mcmc::simulation::SimulationParameters<ONModelGPUParameters<N>>::prepare_simulation_from_file(
            system_params, readable_measures,
            "systembase_params", "kappa", kappa_intervals);

    // Store simulation parameters
    simulation_parameters.write_to_file(path_parameters.get_rel_config_path());
}

template<uint N>
struct EquilibriumTimeSimulation : mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare equilibrium time simulation
        typedef mcmc::mode::EquilibriumTimeParameters EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(20, 100, 0.05, 10, "SecondMoment");
        equilibrium_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>(
                "equilibrium_time", this->path_parameters, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
    }
};

template<uint N>
struct CorrelationTimeSimulation : mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::CorrelationTimeParameters CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(1000, 400, this->path_parameters.get_rel_results_path(), {"SecondMoment"});
        correlation_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>(
                "correlation_time", this->path_parameters, true, true,
                mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
    }
};

template<uint N>
struct ExpectationValueSimulation : mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>
{
    using mcmc::cmdint::CmdIntSim<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>::CmdIntSim;

    void prepare() override
    {
        // Prepare correlation time simulation
        typedef mcmc::mode::ExpectationValueParameters ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
                this->path_parameters.get_rel_results_path(), 200, this->path_parameters.get_rel_results_path(),
                {"Mean", "AbsMean", "SecondMoment", "Action", "Config"}, {}, "hot", "statistical");
        expectation_value_parameters.write_to_file(this->path_parameters.get_rel_config_path());

        // Prepare expectation value simulation on a cluster and submit the job with one function call
        mcmc::cluster::execute<ONModelGPUParameters<N>, mcmc::measures::ReadableMeasureParameters>(
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

    const uint N = 4;
    const std::string target_name = "ONModelSimulation";
    const std::string sim_root_dir = "./";
    const bool rel_path = true;

    prepare_simulation_parameters<N>(target_name, sim_root_dir, rel_path);

    EquilibriumTimeSimulation<N> equilibrium_time_simulation(target_name, sim_root_dir, rel_path);
    equilibrium_time_simulation.main(argc, argv);

    CorrelationTimeSimulation<N> correlation_time_simulation(target_name, sim_root_dir, rel_path);
    correlation_time_simulation.main(argc, argv);

    ExpectationValueSimulation<N> expectation_value_simulation(target_name, sim_root_dir, rel_path);
    expectation_value_simulation.main(argc, argv);


    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}