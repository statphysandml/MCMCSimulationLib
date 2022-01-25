#ifndef VIRTUAL_ENV_INTEGRATION_HPP
#define VIRTUAL_ENV_INTEGRATION_HPP

/** \file virtual_env_integration.hpp
 * Important variables and methods for a correct initialization and usage of a virtual environment with conda/miniconda.
 * Note that this is only needed if the simulation is supposed to be executed on a cluster. In the provided support for
 * executions on a cluster the virtual environment is activated by (see cpu_cluster_integration.cpp).
 * \code{.sh}
 * source conda_activate_path virtual_env
 * \endcode
 * The respective settings can be adapted based on your cluster and used virtual environment. The imporant variables can be
 * set with the help of the cmake variables -DCONDA_ACTIVATE_PATH=... and -DVIRTUAL_ENV=... or manually by the methods given
 * below.
 */

#include <iostream>
#include "execution/config.h"

#ifdef RUN_WITH_VITUAL_ENV

namespace mcmc {
    namespace virtualenv {
            // Global parameters
            /** Used path to activate the virtual environment before executing the MCMC simulation in C++ on a cluster. Per default, this path is set to the cmake variable CONDA_ACTIVATE_PATH which can be set by -DCONDA_ACTIVATE_PATH when the library is built. */
            extern std::string conda_activate_path;
            /** Name of the virtual environment used to execute the MCMC simulation in C++ on a cluster. Per default, this path is set to the cmake variable VIRTUAL_ENV which can be set by -DVIRTUAL_ENV when the library is built. */
            extern std::string virtual_env;

            /** @brief Can be used to set the conda_activate_path to a different than the default one
             * @param conda_activate_path_ Path to conda activate (to the binary called when source activate is executed, for example .../.miniconda3/bin)
             */
            void set_conda_activate_path(const std::string conda_activate_path_);
            /** @brief Can be used to set the virtual environment to a different than the default one
             *  @param virtual_env_ Name of the virtual environment to be activated
             */
            void set_virtual_env(const std::string virtual_env_);
    }
}

#endif


#endif //VIRTUAL_ENV_INTEGRATION