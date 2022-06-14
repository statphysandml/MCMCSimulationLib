#include <mcmc/cluster_support/virtual_env_integration.hpp>

namespace mcmc::virtualenv {
    // Global parameters - Default
    std::string virtual_env_integration::g_conda_activate_path = CONDA_ACTIVATE_PATH;
    std::string virtual_env_integration::g_virtual_env = VIRTUAL_ENV;

    void virtual_env_integration::set_conda_activate_path(std::string_view conda_activate_path) {
        g_conda_activate_path = conda_activate_path;
        std::cout << " -- Setting conda activate path to '" << g_conda_activate_path << "' --" << std::endl;
    }

    void virtual_env_integration::set_virtual_env(std::string_view virtual_env) {
        g_virtual_env = virtual_env;
        std::cout << " -- Setting virtual_env to '" << g_virtual_env << "' --" << std::endl;
    }
}