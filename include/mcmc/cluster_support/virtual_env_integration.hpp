#ifndef VIRTUAL_ENV_INTEGRATION_HPP
#define VIRTUAL_ENV_INTEGRATION_HPP

/** \file The MCMCSimulationLib provides the possibility to pass the conda activate path and the name of the used virtual environment to C++, since knowing these variables can be necessary for generating the bash scripts for a submission to a cluster:
 */

#include <iostream>
#include <mcmc/cluster_support/config.h>

namespace mcmc::virtualenv {
    // Global parameters

    struct virtual_env_integration
    {
        /** Path to conda activate (for example: ``~/miniconda3/bin/activate``). Per default, this path is set to the CMake variable ``CONDA_ACTIVATE_PATH``. */
        static std::string g_conda_activate_path;
        /** Name of the virtual environment to be activated before execution of the script. Per default, this path is set to the CMake variable ``VIRTUAL_ENV``. */
        static std::string g_virtual_env;

        /** @brief Can be used to set the conda_activate_path to a different than the default one.
         * @param conda_activate_path Path to conda activate
         */
        static void set_conda_activate_path(const std::string &conda_activate_path);
        /** @brief Can be used to set the virtual environment to a different than the default one.
         *  @param virtual_env Name of the virtual environment to be activated
         */
        static void set_virtual_env(const std::string &virtual_env);
    };
}


#endif //VIRTUAL_ENV_INTEGRATION