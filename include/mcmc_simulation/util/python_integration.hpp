#ifndef PYTHON_INTEGRATION_HPP
#define PYTHON_INTEGRATION_HPP

/** \file python_integration.hpp Overview of important variables and methods for
 * a correct initialization and usage of Python throughout the project. For a
 * smooth integration, Python needs to be initialized before executing Python
 * code by
 * \code{.cpp}
 * // Initialization
 * #ifdef PYTHON_BACKEND
 * mcmc::execution::initialize_python(PYTHON_SCRIPTS_PATH);
 * #endif
 * \endcode
 * and at the end finalized by
 * \code{.cpp}
 * // Finalization
 * #ifdef PYTHON_BACKEND
 * mcmc::execution::finalize_python();
 * #endif
 * \endcode
 * 
 * The CMake variable ``RUN_WITH_PYTHON_BACKEND`` needs to be set by
 * ``-DRUN_WITH_PYTHON_BACKEND=ON`` (default=``ON``) when the library is built.
 *
 * The optional CMake variable ``PYTHON_SCRIPTS_PATH`` can be set by
 * ``-DPYTHON_SCRIPTS_PATH=<path_to_python_modules>`` when the project is built.
 * The <path_to_python_modules> refers to the directory containing a
 * ``custom_modules.py``file and allowing for an integration of custom measure
 * functions and a custom function for loading MCMC simulation data from file.
 */

#include <iostream>

// #ifdef PYTHON_BACKEND

#include <pybind11/embed.h>
namespace py = pybind11;

#include <param_helper/params.hpp>

namespace mcmc {
    namespace util {
        
        // Project dependent variables
        /** Path to the directory containing the ``custom_modules.py`` file with
         * custom Python code for computing measures and loading MCMC simulation
         * data. */
        extern std::string g_python_modules_path;
        /** Variable for checking if Python has been initialized correctly. */
        extern bool g_is_python_initialized;

        /** @brief Returns the ``g_python_modules_path``. */
        std::string get_python_modules_path();
        /** @brief Sets the ``g_python_modules_path``. */
        void set_python_modules_path(const std::string python_modules_path);

        /** @brief Initialize Python and set important variables
         * 
         * @param python_modules_path Path to a directory containing custom Python code
         * @param fma_develop Indicate whether plots should be written to file or loaded locally
         * @param executing_from_python Variable for indicating whether the code is executed from C++ (false) or executed via pybind from Python (true)
         *
         */
        void initialize_python(const std::string python_modules_path, const bool fma_develop=false, const bool executing_from_python=false);
        /** @brief Finalize Python. */
        void finalize_python();
    }
}

// #endif

#endif //PYTHON_INTEGRATION_HPP
