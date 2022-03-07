#include "../../include/cluster_support/virtual_env_integration.hpp"

namespace mcmc {
    namespace virtualenv {
        // Global parameters - Default
        std::string conda_activate_path = CONDA_ACTIVATE_PATH;
        std::string virtual_env = VIRTUAL_ENV;

        void set_conda_activate_path(const std::string conda_activate_path_) {
            conda_activate_path = conda_activate_path_;
            std::cout << " -- Setting conda activate path to '" << conda_activate_path << "' --" << std::endl;
        }

        void set_virtual_env(const std::string virtual_env_) {
            virtual_env = virtual_env_;
            std::cout << " -- Setting virtual_env to '" << virtual_env << "' --" << std::endl;
        }
    }
}