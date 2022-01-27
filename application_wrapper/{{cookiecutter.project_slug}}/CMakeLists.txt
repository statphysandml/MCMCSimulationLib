cmake_minimum_required(VERSION 3.15)

# Set a name and a version number for your project:
project({{ cookiecutter.project_slug }} VERSION 0.0.1 LANGUAGES CXX)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD {{ cookiecutter.cxx_minimum_standard }})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

{% if cookiecutter.python_bindings == "Yes" -%}
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
{%- endif %}

# Compilation options
{%- if cookiecutter.python_bindings == "Yes" %}
set(BUILD_PYTHON_BINDINGS ON CACHE BOOL "Enable building of Python bindings")
{%- endif %}
{%- if cookiecutter.doxygen == "Yes" or cookiecutter.readthedocs == "Yes" %}
set(BUILD_DOCS ON CACHE BOOL "Enable building of documentation")
{%- endif %}

# General compilation options
set(RUN_WITH_PYTHON_BACKEND ON CACHE BOOL "Use Python backend to compute expectation values, etc.")

# Project specific compliation options (used by config.h.in)
set(CLUSTER_MODE "local" CACHE STRING "Mode to run the cluster.")
set(PYTHON_SCRIPTS_PATH "../python_scripts" CACHE STRING "Path to the potential python scripts evaluated by the python backend (relative to build directory)")

find_library(ParamHelper NAMES libparamhelper.a PATHS {{ cookiecutter.path_to_mcmc_simulation_lib }}/build/ext/ParamHelper/src)
message("ParamHelper = ${ParamHelper}")
include_directories({{ cookiecutter.path_to_mcmc_simulation_lib }}/ext/ParamHelper/include/)

find_library(MCMCSimulationLib NAMES libmcmcsimulationlib.a PATHS {{ cookiecutter.path_to_mcmc_simulation_lib }}/build/src)
message("MCMCSimulationLib = ${MCMCSimulationLib}")
include_directories({{ cookiecutter.path_to_mcmc_simulation_lib }}/include/)


configure_file(./include/{{ cookiecutter.project_slug }}/config.h.in ../include/{{ cookiecutter.project_slug }}/config.h @ONLY)


if(RUN_WITH_PYTHON_BACKEND)
    # Paths relative to path to CMakeLists.txt and relative to build/ path
    file(COPY {{ cookiecutter.path_to_mcmc_simulation_lib }}/ext/pybind11 DESTINATION ../ext)
    add_subdirectory(ext/pybind11)
    set(target_link_libraries_python_appendix "pybind11::embed")
    # Or without pybind11:
    # set(Python3_FIND_VIRTUALENV "FIRST")  
    # find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    # include_directories(${Python3_INCLUDE_DIRS})
    # set(target_link_libraries_python_appendix "${Python3_LIBRARIES}")
    message("Running with python backend enabled.")
else()
    # Use library without the integration of python code
    message("Running without python backend enabled.")
endif()

{% if cookiecutter.build_tests == "Yes" -%}
# compile the tests
include(CTest)
if(BUILD_TESTING)
  {%- if cookiecutter.use_submodules == "Yes" %}
  add_subdirectory(ext/Catch2)
  include(./ext/Catch2/contrib/Catch.cmake)
  {%- else %}
  find_package(Catch2 REQUIRED)
  include(Catch)
  {%- endif %}
  add_subdirectory(tests)
endif()
{%- endif %}

{% if cookiecutter.doxygen == "Yes" -%}
if(BUILD_DOCS)
  # Add the documentation
  add_subdirectory(doc)
endif()
{%- endif %}
{%- if cookiecutter.python_bindings == "Yes" %}
if(BUILD_PYTHON_BINDINGS)
  # Add Python bindings
  {%- if cookiecutter.use_submodules == "Yes" %}
  file(COPY ../ext/pybind11 DESTINATION ../ext)
  add_subdirectory(ext/pybind11)
  {% else %}
  find_package(pybind11)
  {%- endif %}
  add_subdirectory(python)
endif()
{%- endif %}

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)

add_executable({{ cookiecutter.project_slug }} src/main.cpp)
if(RUN_WITH_PYTHON_BACKEND)
  target_compile_definitions({{ cookiecutter.project_slug }} PUBLIC PYTHON_BACKEND)
endif()
target_link_libraries({{ cookiecutter.project_slug }} PUBLIC ${MCMCSimulationLib} ${ParamHelper} ${target_link_libraries_python_appendix})
