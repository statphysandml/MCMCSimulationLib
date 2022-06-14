#include <mcmc/cluster_support/gpu_cluster_integration.hpp>


namespace mcmc::cluster {
    void prepare_execution_on_gpu_cluster(
        const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters,
        const std::string &executable_name, const bool run, const bool eval, const std::vector<std::string> &additional_args) {
        std::cerr << "To be implemented" << std::endl;
    }

    void run_execution_on_gpu_cluster(const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters, std::string_view cluster_mode) {
        std::cerr << "To be implemented" << std::endl;
    }
}
