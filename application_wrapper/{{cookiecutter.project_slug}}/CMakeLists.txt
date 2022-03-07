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
set(PYTHON_SCRIPTS_PATH "./python_scripts" CACHE STRING "Path to the potential python scripts evaluated by the python backend (relative to build directory)")

# Include libraries
find_package(ParamHelper CONFIG REQUIRED)
find_package(MCMCSimulationLib CONFIG REQUIRED)


configure_file(./include/{{ cookiecutter.project_slug }}/config.h.in ../include/{{ cookiecutter.project_slug }}/config.h @ONLY)


if(RUN_WITH_PYTHON_BACKEND)
    set(target_link_libraries_python_appendix "pybind11::embed")
    # Or without pybind11:
    # set(target_link_libraries_python_appendix "${Python3_LIBRARIES}")
    message("Running with python backend enabled.")
else()
    # Use library without the integration of python code
    message("Running with python backend disabled.")
endif()

if((BUILD_PYTHON_BINDINGS OR RUN_WITH_PYTHON_BACKEND) AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11")
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
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ext/pybind11")
    message(FATAL_ERROR "pybind11 was not added to ${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11. Please install git or add pybind11 manually.")
endif()

if(BUILD_PYTHON_BINDINGS)
    message("Integrate python for python_bindings")
    # Paths relative to path to CMakeLists.txt and relative to build/ path
    # file(COPY ../../ext/pybind11 DESTINATION ../ext)
    # Add Python bindings
    add_subdirectory(ext/pybind11)
    # Python wrapper
    add_subdirectory(python_pybind)
elseif(RUN_WITH_PYTHON_BACKEND)
    message("Load pybind11 module")
    # Paths relative to path to CMakeLists.txt and relative to build/ path
    # file(COPY ../../ext/pybind11 DESTINATION ../ext)
    # Add Python bindings for correct binding of pybind11 without usage of pybind11_modules
    add_subdirectory(ext/pybind11)
    # Or without pybind11:
    # set(Python3_FIND_VIRTUALENV "FIRST")
    # find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    # include_directories(${Python3_INCLUDE_DIRS})
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
target_link_libraries({{ cookiecutter.project_slug }} PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper ${target_link_libraries_python_appendix})
