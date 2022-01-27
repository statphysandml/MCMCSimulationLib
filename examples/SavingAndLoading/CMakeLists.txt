cmake_minimum_required(VERSION 3.15)

# Set a name and a version number for your project:
project(SavingAndLoading VERSION 0.0.1 LANGUAGES CXX)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# General compilation options
set(RUN_WITH_PYTHON_BACKEND ON CACHE BOOL "Use Python backend to compute expectation values, etc.")

# Project specific compliation options (used by config.h.in)
set(CLUSTER_MODE "local" CACHE STRING "Mode to run the cluster.")
set(PYTHON_SCRIPTS_PATH "./python_scripts" CACHE STRING "Path to the potential python scripts evaluated by the python backend")

# Include libraries
find_library(ParamHelper NAMES libparamhelper.a PATHS ./../../build/ext/ParamHelper/src)
message("ParamHelper = ${ParamHelper}")
include_directories(../../ext/ParamHelper/include/)

find_library(MCMCSimulationLib NAMES libmcmcsimulationlib.a PATHS ../../build/src)
message("MCMCSimulationLib = ${MCMCSimulationLib}")
include_directories(../../include/)


configure_file(./include/saving_and_loading/config.h.in ../include/saving_and_loading/config.h @ONLY)


if(RUN_WITH_PYTHON_BACKEND)
    # Paths relative to path to CMakeLists.txt and relative to build/ path
    file(COPY ../../ext/pybind11 DESTINATION ../ext)
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
    message("Running with python backend disabled")
endif()

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)

add_executable(SavingAndLoading src/main.cpp)
if(RUN_WITH_PYTHON_BACKEND)
    target_compile_definitions(SavingAndLoading PUBLIC PYTHON_BACKEND)
endif()
target_link_libraries(SavingAndLoading PUBLIC ${MCMCSimulationLib} ${ParamHelper} ${target_link_libraries_python_appendix})
