#ifndef CPU_CLUSTER_INTEGRATION_INTEGRATION_HPP
#define CPU_CLUSTER_INTEGRATION_INTEGRATION_HPP

#include <iostream>

#include <chrono>
#include <thread>

#include <param_helper/params.hpp>

#include "mcmc_simulation/util/path_parameters.hpp"
#include "execution/virtual_env_integration.hpp"

namespace mcmc {
    namespace cluster {
        
        /** @brief Function used by mcmc::cmd::from_file to prepare the bash script for executing a simulation on a cpu cluster. It needs to be adapted according to the used cpu cluster. */
        void prepare_execution_on_cpu_cluster(
            const std::string mode_type,
            const mcmc::cmd::PathParameters path_parameters,
            const std::string executable_name,
            const bool eval = true,
            const bool run = true,
            const std::vector<std::string> additional_args = {}
        );
        
        /** @brief Function used by mcmc::cmd::from_file to submit the job to a cpu cluster. It needs to be adapted according to the used cpu cluster. */
        void run_execution_on_cpu_cluster(
            const std::string mode_type,
            const mcmc::cmd::PathParameters path_parameters,
            const std::string cluster_mode
        );
    }
}

#endif //CPU_CLUSTER_INTEGRATION_INTEGRATION