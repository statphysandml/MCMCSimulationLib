#include "../../include/cluster_support/cluster_integration.hpp"

namespace mcmc {
    namespace cluster {
        // Project dependent parameters
        std::string executable_name = "None";
        std::string cluster_mode = "local";

        void initialize_cluster_params(const std::string executable_name_,
                                       const std::string cluster_mode_) {
            set_executable_name(executable_name_);
            set_cluster_mode(cluster_mode_);
        }

        std::string get_executable_name() {
            return executable_name;
        }

        void set_executable_name(const std::string executable_name_) {
            executable_name = executable_name_;
            std::cout << " -- Setting executable name to '" << executable_name << "' --" << std::endl;
        }

        std::string get_cluster_mode() {
            return cluster_mode;
        }

        void set_cluster_mode(const std::string cluster_mode_) {
            cluster_mode = cluster_mode_;
            std::cout << " -- Setting cluster mode to '" << cluster_mode << "' --" << std::endl;
        }
    }
}
