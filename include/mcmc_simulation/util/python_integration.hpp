#ifndef PYTHON_INTEGRATION_HPP
#define PYTHON_INTEGRATION_HPP

/** \file python_integration.hpp
 * Important variables and methods for a correct initialization and usage of Python throughout the project.
 * For a smooth integration, Python needs to be initialized at the beginning by
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
 * The cmake variable RUN_WITH_PYTHON_BACKEND needs to be set by -DRUN_WITH_PYTHON_BACKEND=ON (default=ON)
 * when the library is built. And the cmake variable PYTHON_SCRIPTS_PATH can be set by -DPYTHON_SCRIPTS_PATH=<path_to_python_modules>
 * when the application/project is built. The <path_to_python_modules> refers to a directory containing Python code that is supposed to be used during the simulation.
 * See the examples or the the project generated by python generate_application for an example.
 */

#include <iostream>

// #ifdef PYTHON_BACKEND

#include <pybind11/embed.h>
namespace py = pybind11;

namespace mcmc {
    namespace util {
        
        // Project dependent variables
        /** Path to a directory containing custom python code for computing measures, etc. the modules are imported in python in ... */
        extern std::string python_modules_path;
        /** Variable for checking if Python has been initialized correctly. */
        extern bool is_python_initialized;

        /** @brief Returns the python_modules_path. */
        std::string get_python_modules_path();
        /** @brief Sets the python_modules_path. */
        void set_python_modules_path(const std::string python_modules_path_);

        /** @brief Initialize Python and set important variables
         * 
         * @param python_modules_path_ Path to a directory containing custom python code
         * @param fma_develop Indicate whether plots should be written to file or loaded locally
         * @param executing_from_python Variable for indicating whether the code is executed from C++ (false) or executed via pybind from Python (true)
         *
         */
        void initialize_python(const std::string python_modules_path_, const bool fma_develop=false, const bool executing_from_python=false);
        /** @brief Finalize Python. */
        void finalize_python();
    }
}

// #endif

#endif //PYTHON_INTEGRATION_HPP