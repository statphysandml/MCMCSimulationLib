#include "../../include/cluster_support/virtual_env_integration.hpp"

namespace mcmc {
    namespace virtualenv {
        // Global parameters - Default
        std::string g_conda_activate_path = CONDA_ACTIVATE_PATH;
        std::string g_virtual_env = VIRTUAL_ENV;

        void set_conda_activate_path(const std::string conda_activate_path) {
            g_conda_activate_path = conda_activate_path;
            std::cout << " -- Setting conda activate path to '" << g_conda_activate_path << "' --" << std::endl;
        }

        void set_virtual_env(const std::string virtual_env) {
            g_virtual_env = virtual_env;
            std::cout << " -- Setting virtual_env to '" << g_virtual_env << "' --" << std::endl;
        }
    }
}