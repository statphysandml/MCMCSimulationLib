cmake_minimum_required(VERSION 3.15)

# Set a name and a version number for your project:
project(MCMCSimulationLib VERSION 0.0.1 LANGUAGES CXX)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compilation options
set(BUILD_PYTHON_BINDINGS ON CACHE BOOL "Enable building of Python bindings")
set(RUN_WITH_PYTHON_BACKEND ON CACHE BOOL "Use Python backend to compute expectation values, etc.")
set(BUILD_DOCS ON CACHE BOOL "Enable building of documentation")

# Additional compliation options to be able to activate the environment before the actual simulation in a generated bash script.
# These are only required for runs with a queue-script. (used by include/execution/config.h.in)
set(VIRTUAL_ENV "None" CACHE STRING "Name of the virtual environment")
set(CONDA_ACTIVATE_PATH "None" CACHE STRING "Path to conda activate path-to-conda-activate/bin/activate")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ext/ParamHelper/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

configure_file(./include/execution/config.h.in ../include/execution/config.h @ONLY)

# Compilation options

# compile the external submodule
add_subdirectory(ext/ParamHelper)
include_directories(ext/ParamHelper/include)

if(BUILD_PYTHON_BINDINGS)
    message("Integrate python for python_bindings")
    # Add Python bindings
    add_subdirectory(ext/pybind11)

    # Python virtual environment
    add_subdirectory(python)
elseif(RUN_WITH_PYTHON_BACKEND)
    message("Load pybind11 module")
    # Add Python bindings for correct binding of pybind11 without usage of pybind11_modules
    add_subdirectory(ext/pybind11)
    # Or without pybind11:
    # set(Python3_FIND_VIRTUALENV "FIRST")
    # find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    # include_directories(${Python3_INCLUDE_DIRS})
endif()

if(RUN_WITH_PYTHON_BACKEND)
    set(target_link_libraries_python_appendix "pybind11::embed")
    # Or without pybind11:
    # set(target_link_libraries_python_appendix "${Python3_LIBRARIES}")
    message("Running with python backend enabled.")
    if(NOT VIRTUAL_ENV EQUAL "None" AND NOT CONDA_ACTIVATE_PATH EQUAL "None")
        message("Enable queue-script generation with virtual environment")
        option(RUN_WITH_VIRTUAL_ENV "Enable queue-script generation with virtual environment" ON)
    else()
        message("Disable queue-script generation with virtual environment")
        option(RUN_WITH_VIRTUAL_ENV "Disable queue-script generation with virtual environment" OFF)
    endif()
else()
    # Use library without the integration of python code
    message("Running with python backend disabled.")
endif()

# compile the library
add_subdirectory(src)


# compile the tests
include(CTest)
if(BUILD_TESTING)
  # add_subdirectory(ext/ParamHelper/ext/Catch2)
  include(./ext/ParamHelper/ext/Catch2/contrib/Catch.cmake)
  add_subdirectory(tests)
endif()

if(BUILD_DOCS)
    # Add the documentation
    add_subdirectory(doc)
endif()


# Add an alias target for use if this project is included as a subproject in another project
add_library(mcmcsimulationlib::mcmcsimulationlib ALIAS mcmcsimulationlib)

# Install targets and configuration
install(
  TARGETS mcmcsimulationlib
  EXPORT mcmcsimulationlib-config
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT mcmcsimulationlib-config
  NAMESPACE mcmcsimulationlib::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcmcsimulationlib
)

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
