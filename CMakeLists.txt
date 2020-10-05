cmake_minimum_required(VERSION 3.16)
project(MCMCSimulationLib)

set(CMAKE_CXX_STANDARD 14)

# Python

# Home
set(PYTHON_LIBRARIES "/home/lukas/.miniconda3/envs/flowequation/lib/libpython3.7m.so")
set(PYTHON_EXECUTABLE "/home/lukas/.miniconda3/envs/flowequation/bin/python3.7m")
include_directories("/home/lukas/.miniconda3/envs/flowequation/include/python3.7m")
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# Uni
# include_directories("/home/kades/.conda/envs/pytorchlocal3/include/python3.7m")
# set(PYTHON_LIBRARIES "/home/kades/.conda/envs/pytorchlocal3/lib/libpython3.7m.so")
# set(PYTHON_EXECUTABLE "/home/kades/.conda/envs/pytorchlocal3/bin/python3.7m")
# find_package(PythonInterp 3 REQUIRED)

message("Python executable = ${PYTHON_EXECUTABLE}")

add_library(mcmcsimulationlib STATIC
        src/mcmc_simulation/simulation.cpp
        src/mcmc_simulation/util/random.cpp
        src/mcmc_simulation/sampler/gaussian_sampler.cpp
        src/execution/executer.cpp)

find_library(ParamHelper NAMES libparamhelper.a PATHS /home/lukas/ParamHelper/lib)
if(ParamHelper)
    # include_directories(${Param_Helper})
    message("ParamHelper = ${ParamHelper}")
    include_directories(/home/lukas/ParamHelper/include/)
    target_link_libraries(mcmcsimulationlib ${PYTHON_LIBRARIES} ${ParamHelper})
endif()

# CMake instructions to test using the static lib

SET( APP_EXE StaticTest )

ADD_EXECUTABLE( ${APP_EXE}
        src/main.cpp )

TARGET_LINK_LIBRARIES( ${APP_EXE}
        mcmcsimulationlib)
