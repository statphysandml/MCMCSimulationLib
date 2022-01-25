#ifndef GPU_CLUSTER_INTEGRATION_INTEGRATION_HPP
#define GPU_CLUSTER_INTEGRATION_INTEGRATION_HPP

#include <iostream>

#include "command_line_support/path_parameters.hpp"

namespace mcmc {
    namespace cluster {
        
        /** @brief Function used by mcmc::cmdint::from_file to prepare the bash script for executing a simulation on a gpu cluster. It needs to be adapted according to the used gpu cluster. */
        void prepare_execution_on_gpu_cluster(
            const std::string mode_type,
            const mcmc::cmdint::PathParameters path_parameters,
            const std::string executable_name,
            const bool eval = true,
            const bool run = true,
            const std::vector<std::string> additional_args = {}
        );
        
        /** @brief Function used by mcmc::cmdint::from_file to submit the job to a gpu cluster. It needs to be adapted according to the used gpu cluster. */
        void run_execution_on_gpu_cluster(
            const std::string mode_type,
            const mcmc::cmdint::PathParameters path_parameters,
            const std::string cluster_mode
        );
    }
}

#endif //GPU_CLUSTER_INTEGRATION_INTEGRATION