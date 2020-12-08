cat >../CMakeLists.txt <<EOL
cmake_minimum_required(VERSION 3.13)
project(MCMCSimulationLib)

set(CMAKE_CXX_STANDARD 14)

# Boost
EOL
if [ -v path_to_boost ]; then
cat >>../CMakeLists.txt <<EOL
set(BOOST_ROOT "${path_to_boost}")
FIND_PACKAGE( Boost REQUIRED COMPONENTS filesystem)
EOL
else
cat >>../CMakeLists.txt <<EOL
FIND_PACKAGE( Boost 1.67 REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(\${Boost_INCLUDE_DIRS})
    message("Boost = \${Boost_INCLUDE_DIRS}")
endif()
EOL
fi
cat >>../CMakeLists.txt <<EOL

EOL
if [ -v path_to_python3 ]; then
cat >>../CMakeLists.txt <<EOL
# Python
set(PYTHON_LIBRARIES "${path_to_python3}lib/libpython${python_version}m.so")
set(PYTHON_EXECUTABLE "${path_to_python3}bin/python${python_version}m")
set(Python3_ROOT_DIR "$path_to_python3")
include_directories("${path_to_python3}include/python${python_version}m")
# find_package(PythonInterp 3 REQUIRED) # Not working at the university
find_package(PythonLibs 3 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
include_directories(\${PYTHON_INCLUDE_DIRS})
message("Python executable = \${PYTHON_EXECUTABLE}")
option( PYTHON "Enable Python" ON)
EOL
else
cat >>../CMakeLists.txt <<EOL
option( PYTHON "Disable Python" OFF)
EOL
fi
cat >>../CMakeLists.txt <<EOL

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -static-libstdc++ -lboost_system") 

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

EOL
if [ -v path_to_python3 ]; then
cat >>../CMakeLists.txt <<EOL
target_link_libraries(mcmcsimulationlib \${ParamHelper} \${Boost_LIBRARIES} \${PYTHON_LIBRARIES})
EOL
ficat >>../CMakeLists.txt <<EOL

SET( APP_EXE StaticTest )

ADD_EXECUTABLE( \${APP_EXE}
        src/main.cpp )
        
TARGET_LINK_LIBRARIES( \${APP_EXE}
        mcmcsimulationlib)

EOL
