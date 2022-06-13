#include <mcmc/cluster_support/cluster_integration.hpp>

namespace mcmc::cluster {
    // Project dependent parameters
    std::string cluster_integration::g_executable_name = "None";
    std::string cluster_integration::g_cluster_mode = "local";

    void initialize_cluster_params(const std::string &executable_name,
                                    const std::string &cluster_mode) {
        set_executable_name(executable_name);
        set_cluster_mode(cluster_mode);
    }

    std::string get_executable_name() {
        return cluster_integration::g_executable_name;
    }

    void set_executable_name(const std::string &executable_name) {
        cluster_integration::g_executable_name = executable_name;
        std::cout << " -- Setting executable name to '" << cluster_integration::g_executable_name << "' --" << std::endl;
    }

    std::string get_cluster_mode() {
        return cluster_integration::g_cluster_mode;
    }

    void set_cluster_mode(const std::string &cluster_mode) {
        cluster_integration::g_cluster_mode = cluster_mode;
        std::cout << " -- Setting cluster mode to '" << cluster_integration::g_cluster_mode << "' --" << std::endl;
    }
}
