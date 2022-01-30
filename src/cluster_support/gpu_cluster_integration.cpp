#include "../../include/cluster_support/gpu_cluster_integration.hpp"


namespace mcmc {
    namespace cluster {
        void prepare_execution_on_gpu_cluster(
            const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters,
            const std::string executable_name, const bool eval, const bool run, const std::vector<std::string> additional_args) {
            std::cerr << "To be implemented" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        void run_execution_on_gpu_cluster(const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters, const std::string cluster_mode) {
            std::cerr << "To be implemented" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}