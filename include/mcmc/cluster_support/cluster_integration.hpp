#ifndef CLUSTER_INTEGRATION_HPP
#define CLUSTER_INTEGRATION_HPP


#include <iostream>
#include <map>
#include <utility>

#include <mcmc/cluster_support/config.h>

#include <mcmc/command_line_support/path_parameters.hpp>
#include <mcmc/command_line_support/cmdint.hpp>

#include <mcmc/cluster_support/cpu_cluster_integration.hpp>
#include <mcmc/cluster_support/gpu_cluster_integration.hpp>
#include <mcmc/cluster_support/virtual_env_integration.hpp>

namespace mcmc::cluster {

    /** @brief Function for intializing project dependent parameters for an
     * execution of the MCMC simulation on a cluster.
     *
     * If you generate your project with the ``generate_application.py``
     * script, the ``CLUSTER_MODE`` CMake variable can be set when calling
     * ``cmake``. In this setting, the recommended way to initialize the
     * cluster parameters is to add the following line at the beginning of
     * your ``main`` function.
     * \code{.cpp}
     * mcmc::cluster::initialize_cluster_params(PROJECT_NAME, CLUSTER_MODE);
     * \endcode
     * The two variables, PROJECT_NAME and the CLUSTER_MODE are defined in
     * the ``config.h`` file in the ``include/`` which is automatically
     * generated by cmake.
     *
     * @param executable_name Name of the executable
     * @param cluster_mode Variable allowing to test the submission to the
     * cluster Device::locally before an actual submission to the cluster. By
     * setting ``cluster_mode="local"``, the job will be started by
     * executing
     * \code{.sh}
     * nice -n 17 bash <name_of_the_job_file>.sh.
     * \endcode
     * With ``cluster_mode="on_cluster"``, the job will be submitted to the
     * cluster (for more details, see also
     * ``mcmc::cluster::run_execution_on_cpu_cluster`` and
     * ``mcmc::cluster::run_execution_on_gpu_cluster``).
     */
    void initialize_cluster_params(const std::string &executable_name, const std::string &cluster_mode);

    // Global parameters
    struct cluster_integration
    {
        static std::string g_executable_name;
        static std::string g_cluster_mode;

        static std::string get_executable_name();
        static void set_executable_name(std::string_view executable_name);

        static std::string get_cluster_mode();
        static void set_cluster_mode(std::string_view cluster_mode);
    };

    // Managing device
    enum class Device {
        locally, on_gpu_cluster, on_cpu_cluster
    };

    // Managing running mode
    enum class RunningMode {
        prep, exec, prep_and_exec
    };

    /** @brief Main function for preparing and executing MCMC simulations
     * on a cluster
     *
     * Note that the function supports with the help of ``device=Device::locally``
     * the possibility to execute exactly the same code as the one being
     * executed on the cluster beforehand Device::locally.
     *
     * @param mode_type The execution mode used for the simulation
     * (``"equilibrium_time"``, ``"correlation_time"`` or
     * ``"expectation_value"``)
     * @param path_parameters Path parameters containing all important paths
     * for finding the necessary configuration files and for storing data
     * and results
     * @param run Indicate whether the MCMC simulation is performed
     * (``true``) or not (``false``)
     * @param eval Indicate whether the evaluation should be performed in
     * the same run (``true``) or not (``false``)
     * @param device Indicate the device to be used (``Device::locally``,
     * ``Device::on_gpu_cluster`` or ``Device::on_cpu_cluster``) by choosing ``Device::locally``, it
     * can be checked if the simulation runs properly before an actual
     * simulation on the cluster. This is in particular useful to avoid
     * unnecessary submissions to the cluster.
     * @param running_mode Indicate whether a submission on the cluster
     * should be:
     *      - prepared (``RunningMode::prep``), referring to a generation of the ``.sh``
     *        file for execution,
     *      - executed (``RunningMode::exec``), referring to a submission of the job to
     *        the cluster,
     *      - or prepared and executed (``RunningMode::prep_and_exec``), referring to a
     *        generation of the ``.sh`` file and a subsequent submission to
     *        the cluster.
     * @param addition_args List of optional additional args to be passed to
     * the C++ executable when the simulation is executed.
     */
    template<typename SBP, typename MS>
    void execute(const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters,
        const bool run = true, const bool eval = true,
        const Device device = Device::locally,
        const RunningMode running_mode = RunningMode::prep_and_exec,
        const std::vector<std::string>& additional_args = {}
    )
    {
        if (device == Device::locally) {
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::prep)
            {
                // -- Note, if Device::locally is used as running device, no preparation is necessary. Change running_mode to exec instead --
            }
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::exec)
            {
                // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
                mcmc::cmdint::execute<SBP, MS>(mode_type, path_parameters, run, eval);
            }
        }
        else if (device == Device::on_cpu_cluster) {
            // Ensure that all necessary variables are set!
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::prep)
                mcmc::cluster::prepare_execution_on_cpu_cluster(
                    mode_type, path_parameters, cluster_integration::get_executable_name(), run, eval, additional_args);
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::exec)
                mcmc::cluster::run_execution_on_cpu_cluster(mode_type, path_parameters, cluster_integration::get_cluster_mode());
        } else if (device == Device::on_gpu_cluster) {
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::prep)
                mcmc::cluster::prepare_execution_on_gpu_cluster(
                    mode_type, path_parameters, cluster_integration::get_executable_name(), run, eval, additional_args);
            if (running_mode == RunningMode::prep_and_exec || running_mode == RunningMode::exec)
                mcmc::cluster::run_execution_on_gpu_cluster(mode_type, path_parameters, cluster_integration::get_cluster_mode());
        }
    }
}

// ./{Main} {mode_type} {target_name} {sim_root_dir="/data/"} {rel_path=true}

#endif //CLUSTER_INTEGRATION_HPP
