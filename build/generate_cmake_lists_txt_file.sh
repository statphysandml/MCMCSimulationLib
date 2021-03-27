target_link_libraries_appendix="\${ParamHelper}"
cat >../CMakeLists.txt <<EOL
cmake_minimum_required(VERSION 3.10)
project(MCMCSimulationLib)

set(CMAKE_CXX_STANDARD 14)

EOL
if [ -v path_to_python3 ]; then
target_link_libraries_appendix="${target_link_libraries_appendix} \${PYTHON_LIBRARIES}"
cat >>../CMakeLists.txt <<EOL
# Python
set(PYTHON_LIBRARIES "${path_to_python3}lib/libpython3.7m.so")
set(PYTHON_EXECUTABLE "${path_to_python3}bin/python3.7m")
set(Python3_ROOT_DIR "${path_to_python3}")
set(PYTHON_INCLUDE_DIRS "${path_to_python3}include/python3.7m")
include_directories(\${PYTHON_INCLUDE_DIRS})
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
message("Python executable = \${PYTHON_EXECUTABLE}")

option(PYTHON "Enable Python" ON)
set(VIRTUAL_ENV "${virtual_env}")
set(CONDA_ACTIVATE_PATH "${path_to_conda_activate}")

EOL
else
cat >>../CMakeLists.txt <<EOL
option(PYTHON "Disable Python" OFF)
EOL
fi
cat >>../CMakeLists.txt <<EOL

configure_file(./include/execution/config.h.in ../include/execution/config.h @ONLY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -static-libstdc++")

add_library(mcmcsimulationlib STATIC
        src/mcmc_simulation/simulation.cpp
        src/mcmc_simulation/util/random.cpp
        src/mcmc_simulation/sampler/gaussian_sampler.cpp
        src/mcmc_simulation/sampler/hat_function_sampler.cpp
        src/mcmc_simulation/sampler/uniform_sampler.cpp
        src/execution/executer.cpp)

        
find_library(ParamHelper NAMES libparamhelper.a PATHS ${path_to_param_helper}lib)
message("ParamHelper = \${ParamHelper}")
include_directories(${path_to_param_helper}include/)
if (PYTHON)
  target_compile_definitions(mcmcsimulationlib PUBLIC -D PYTHON)
endif()
target_link_libraries(mcmcsimulationlib ${target_link_libraries_appendix})


SET( APP_EXE StaticTest )

ADD_EXECUTABLE( \${APP_EXE}
        src/main.cpp )

TARGET_LINK_LIBRARIES( \${APP_EXE}
        mcmcsimulationlib)

EOL
