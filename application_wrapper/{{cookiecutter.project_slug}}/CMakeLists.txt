cmake_minimum_required(VERSION 3.15)

# Set a name and a version number for your project:
{%- if cookiecutter.cuda_support == "No" %}
project({{ cookiecutter.project_name }} VERSION 0.0.1 LANGUAGES CXX)
{%- else %}
project({{ cookiecutter.project_name }} VERSION 0.0.1 LANGUAGES CXX CUDA)
{%- endif %}

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD {{ cookiecutter.cxx_minimum_standard }})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

{% if cookiecutter.python_bindings == "Yes" -%}
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
{%- endif %}

# General compilation options
{%- if cookiecutter.python_bindings == "Yes" %}
set(BUILD_PYTHON_BINDINGS ON CACHE BOOL "Enable building of Python bindings")
{%- endif %}
set(RUN_WITH_PYTHON_BACKEND ON CACHE BOOL "Use Python backend to compute expectation values, etc.")

{%- if cookiecutter.doxygen == "Yes" or cookiecutter.readthedocs == "Yes" %}
set(BUILD_DOCS ON CACHE BOOL "Enable building of documentation")
{%- endif %}

# Project specific compliation options (used by config.h.in)
set(CLUSTER_MODE "local" CACHE STRING "Mode to run the cluster.")
set(PYTHON_SCRIPTS_PATH "./python_scripts" CACHE STRING "Path to the potential python scripts evaluated by the python backend.")

# Include libraries
find_package(ParamHelper CONFIG REQUIRED)
find_package(MCMCSimulationLib CONFIG REQUIRED)

{%- if cookiecutter.cuda_support == "Yes" %}
SET(CudaUsage "OFF" CACHE STRING "Defines whether code is executed on GPU or CPU with thrust.")

if( CudaUsage MATCHES "GPU" )
    option( GPU "Enable GPU" ON )
    message("GPU = ${GPU}")
else()
    option( GPU "Enable GPU" OFF )
    message("GPU = ${GPU}")
endif()
{%- endif %}

configure_file(./include/{{ cookiecutter.project_slug }}/config.h.in ../include/{{ cookiecutter.project_slug }}/config.h @ONLY)

if(RUN_WITH_PYTHON_BACKEND)
    set(target_link_libraries_python_appendix "pybind11::embed")
    message("Running with python backend enabled.")
else()
    # Use library without the integration of python code
    message("Running with python backend disabled.")
endif()

{%- if cookiecutter.python_bindings == "Yes" %}
if((BUILD_PYTHON_BINDINGS OR RUN_WITH_PYTHON_BACKEND) AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11")
{%- else %}
if(RUN_WITH_PYTHON_BACKEND AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11")
{%- endif %}
    find_package(Git QUIET)
    if(GIT_FOUND)
        message("Downloading pybind11 into ${CMAKE_CURRENT_SOURCE_DIR}/ext")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext")
        execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/pybind/pybind11.git
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext"
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git clone https://github.com/pybind/pybind11.git failed with ${GIT_SUBMOD_RESULT}.")
        endif()
        execute_process(COMMAND ${GIT_EXECUTABLE} checkout "v2.6.2"
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11"
                        RESULT_VARIABLE GIT_SUBMOD_RESULT OUTPUT_QUIET ERROR_QUIET)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git checkout \"v2.6.2\" failed with ${GIT_SUBMOD_RESULT}.")
        endif()
    endif()

    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ext/pybind11")
        message(FATAL_ERROR "pybind11 was not added to ${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11. Please install git or add pybind11 manually.")
    endif()
endif()

{%- if cookiecutter.python_bindings == "Yes" %}
if(BUILD_PYTHON_BINDINGS)
    message("Integrate python for python_bindings")
    # Add Python bindings
    add_subdirectory(ext/pybind11)
    # Python wrapper
    add_subdirectory(python_pybind)
else{%- endif %}if(RUN_WITH_PYTHON_BACKEND)
    message("Load pybind11 module")
    # Add Python bindings for correct binding of pybind11 without usage of pybind11_modules
    add_subdirectory(ext/pybind11)
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

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)

{%- if cookiecutter.cuda_support == "No" %}
add_executable({{ cookiecutter.project_name }} src/main.cpp)
{%- else %}
add_executable({{ cookiecutter.project_name }} src/main.cpp src/{{ cookiecutter.project_slug }}_helper.cu)
{%- endif %}

if(RUN_WITH_PYTHON_BACKEND)
  target_compile_definitions({{ cookiecutter.project_name }} PUBLIC PYTHON_BACKEND)
endif()
{%- if cookiecutter.cuda_support == "Yes" %}
if (GPU)
    target_compile_definitions({{ cookiecutter.project_name }} PUBLIC -D GPU)
endif()
set_target_properties({{ cookiecutter.project_name }} PROPERTIES CUDA_SEPARABLE_COMPILATION ON CUDA_STANDARD 14)
{%- endif %}
target_link_libraries({{ cookiecutter.project_name }} PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper ${target_link_libraries_python_appendix})
