cmake_minimum_required(VERSION 3.13)
project(MCMCSimulationLib)

set(CMAKE_CXX_STANDARD 14)

# Python
set(PYTHON_LIBRARIES "/remote/lin34/kades/.conda/envs/pytorchlocal3/lib/libpython3.8m.so")
set(PYTHON_EXECUTABLE "/remote/lin34/kades/.conda/envs/pytorchlocal3/bin/python3.8m")
set(Python3_ROOT_DIR "/remote/lin34/kades/.conda/envs/pytorchlocal3/")
include_directories("/remote/lin34/kades/.conda/envs/pytorchlocal3/include/python3.8m")
# find_package(PythonInterp 3 REQUIRED) # Not working at the university
find_package(PythonLibs 3 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
include_directories(${PYTHON_INCLUDE_DIRS})
message("Python executable = ${PYTHON_EXECUTABLE}")


add_library(mcmcsimulationlib STATIC
        src/mcmc_simulation/simulation.cpp
        src/mcmc_simulation/util/random.cpp
        src/mcmc_simulation/sampler/gaussian_sampler.cpp
        src/mcmc_simulation/sampler/hat_function_sampler.cpp
        src/mcmc_simulation/sampler/uniform_sampler.cpp
        src/execution/executer.cpp)

        
find_library(ParamHelper NAMES libparamhelper.a PATHS /remote/lin34/kades/ParamHelper/lib)
message("ParamHelper = ${ParamHelper}")
include_directories(/remote/lin34/kades/ParamHelper/include/)
target_link_libraries(mcmcsimulationlib ${PYTHON_LIBRARIES} ${ParamHelper})


# CMake instructions to test using the static lib

SET( APP_EXE StaticTest )

ADD_EXECUTABLE( ${APP_EXE}
        src/main.cpp )

TARGET_LINK_LIBRARIES( ${APP_EXE}
        mcmcsimulationlib)
