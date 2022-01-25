//
// Created by lukas on 28.02.20.
//

#ifndef CLUSTER_INTEGRATION_HPP
#define CLUSTER_INTEGRATION_HPP


#include <iostream>
#include <map>
#include <utility>

#include "config.h"

#include "command_line_support/path_parameters.hpp"
#include "command_line_support/cmd.hpp"

#include "cluster_support/cpu_cluster_integration.hpp"
#include "cluster_support/gpu_cluster_integration.hpp"
#include "cluster_support/virtual_env_integration.hpp"

namespace mcmc {
    namespace cluster {
        
        /** @brief Function for intializing project dependent parameters for an execution of the MCMC simulation on a cluster.
         * 
         * If you generate your project with python generate_application.py, the cluster mode can be set when calling cmake
         * by -DCLUSTER_MODE=... and the variable EXECUTABLE name is set by cmake automatically. The two variables can be used
         * in your application to initialize the cluster parameters
         * \code{.cpp}
         * mcmc::cluster::initialize_cluster_params(PROJECT_NAME, CLUSTER_MODE);
         * \endcode
         * Note that it is import to include the local config.h file of your the generated project for this to work.
         * 
         * @param executable_name_ Name of the executable - Used to execute the correct executable when jobs are submitted to the cluster
         * @param cluster_mode_ ("local" or "on_cluster") Variable for testing the simulation locally by setting cluster_mode="local".
         * The job will be started by executing
         * \code{.sh}
         * nice -n 17 bash <name_of_the_job_file>.sh.
         * \endcode
         * By setting cluster_mode="on_cluster", the job will be submitted to the cluster (see mcmc::cluster::run_execution_on_cpu_cluster and mcmc::cluster::run_execution_on_gpu_cluster for more details)
         */
        void initialize_cluster_params(const std::string executable_name_, const std::string cluster_mode_);

        std::string get_executable_name();
        void set_executable_name(const std::string executable_name_);

        std::string get_cluster_mode();
        void set_cluster_mode(const std::string cluster_mode_);

        // Managing device
        enum Device {
            locally, on_gpu_cluster, on_cpu_cluster
        };
    
        // Managing running mode
        enum RunningMode {
            prep, exec, prep_and_exec
        };

        /** @brief Function for preparing and executing MCMC simulations on a cluster
         *
         * Note that the function supports with the help of device=locally the possibility to execute exactly the same code as the one being executed on the cluster.
         * 
         * @param mode_type Execution mode to be simulated ("expectation_value", "correlation_time", "equilibrium_time")
         * @param path_parameters Path parameters containing all important paths for finding the necessary config files and for storing data and results
         * @param run Indicate whether the MCMC simulation is performed (true) or not (false)
         * @param eval Indicate whether the evaluation should be performed in the same run (true) or not (false)
         * @param device Indicate the device to be used (locally, on_gpu_cluster, on_cpu_cluster) by choosing locally, it can be checked if the simulation runs properly before an actual simulation on the cluster. This is in particular useful to avoid unnecessary submissions to the cluster.
         * @param running_mode Indicate whether a submission on the cluster should be:
         *      - prepared (prep), referring to a generation of the .sh file for exeuction,
         *      - executed (prep), referring to a submission of the job to the cluster,
         *      - or prepared and executed (prep_and_exec), referring to a generation of the .sh file and a subsequent submission to the cluster.
         * @param addition_args List of additional args to be passed to the C++ executable when the simulation is executed.
         */
        template<typename SBP>
        void execute(const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters,
            const bool run = true, const bool eval = true,
            const Device device = locally,
            const RunningMode running_mode = prep_and_exec,
            const std::vector<std::string> additional_args = {}
        )
        {
            if (device == locally) {
                if (running_mode == prep_and_exec or running_mode == prep)
                {
                    std::cout << "-- Note, if locally is used as running device, no preparation is necessary. Change running_mode to exec instead -- " << std::endl;
                }
                if (running_mode == prep_and_exec or exec)
                {
                    // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
                    mcmc::cmdint::execute<SBP>(mode_type, path_parameters, run, eval);
                }
            }
            else if (device == on_cpu_cluster) {
                // Ensure that all necessary variables are set!
                if (running_mode == prep_and_exec or running_mode == prep)
                    mcmc::cluster::prepare_execution_on_cpu_cluster(
                        mode_type, path_parameters, get_executable_name(), run, eval, additional_args);
                if (running_mode == prep_and_exec or running_mode == exec)
                    mcmc::cluster::run_execution_on_cpu_cluster(mode_type, path_parameters, get_cluster_mode());
            } else if (device == on_gpu_cluster) {
                if (running_mode == prep_and_exec or running_mode == prep)
                    mcmc::cluster::prepare_execution_on_gpu_cluster(
                        mode_type, path_parameters, get_executable_name(), run, eval, additional_args);
                if (running_mode == prep_and_exec or running_mode == exec)
                    mcmc::cluster::run_execution_on_gpu_cluster(mode_type, path_parameters, get_cluster_mode());
            }
        }
    }
}

// ./{Main} {mode_type} {target_name} {sim_root_dir="/data/"} {rel_path=true}

#endif //CLUSTER_INTEGRATION_HPP
