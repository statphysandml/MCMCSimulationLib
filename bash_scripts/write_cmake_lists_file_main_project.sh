target_link_libraries_appendix="\${ParamHelper}"
cat >$project_path/cmake/CMakeLists.txt <<EOL
cmake_minimum_required(VERSION 3.9)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)

EOL
if [ -v path_to_python3 ]; then
target_link_libraries_appendix="${target_link_libraries_appendix} \${PYTHON_LIBRARIES}"
cat >>$project_path/cmake/CMakeLists.txt <<EOL
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

set(PYTHON_SCRIPTS_PATH "${path_to_mcmc_simulation_lib}/python_scripts/")

option(PYTHON "Enable Python" ON)

if(NOT CONDA_ACTIVATE_PATH)
    set(CONDA_ACTIVATE_PATH "${path_to_conda_activate}")
endif()

if(NOT VIRTUAL_ENV)
    set(VIRTUAL_ENV "${virtual_env}")
endif()
EOL
else
cat >>$project_path/cmake/CMakeLists.txt <<EOL
option(PYTHON "Disable Python" OFF)
EOL
fi
cat >>$project_path/cmake/CMakeLists.txt <<EOL

if(NOT CLUSTER_MODE)
    set(CLUSTER_MODE "${cluster_mode}") # else local
endif()

EOL
if [ "$project_path" = "../" ]; then
cat >>$project_path/cmake/CMakeLists.txt <<EOL
configure_file(./../include/config.h.in ./../include/config.h @ONLY)
EOL
else
cat >>$project_path/cmake/CMakeLists.txt <<EOL
configure_file(./../include/config.h.in ./../include/config.h @ONLY)
EOL
fi
cat >>$project_path/cmake/CMakeLists.txt <<EOL

find_library(ParamHelper NAMES libparamhelper.a PATHS ${path_to_param_helper}lib)
message("ParamHelper = \${ParamHelper}")
include_directories(${path_to_param_helper}include/)


find_library(MCMCSimulationLib NAMES libmcmcsimulationlib.a PATHS ${path_to_mcmc_simulation_lib}/lib)
message("MCMCSimulationLib = \${MCMCSimulationLib}")
include_directories(${path_to_mcmc_simulation_lib}/include/)

set(CMAKE_CXX_FLAGS "\${CMAKE_CXX_FLAGS} -std=c++14 -static-libstdc++")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

if( CMAKE_BUILD_TYPE MATCHES "RELEASE" )
    set(CMAKE_EXE_LINKER_FLAGS "-s")  # Strip binary
endif()

add_executable(
    ${project_name}
EOL
if [ "$project_path" = "../" ]; then
cat >>$project_path/cmake/CMakeLists.txt <<EOL
    ./../src/main.cpp
EOL
else
cat >>$project_path/cmake/CMakeLists.txt <<EOL
    ./../src/main.cpp
EOL
fi
cat >>$project_path/cmake/CMakeLists.txt <<EOL
)

target_link_libraries(${project_name} \${MCMCSimulationLib} ${target_link_libraries_appendix})

# Go to build directory and call
# cmake ../cmake/ -DCMAKE_BUILD_TYPE=Release
# make -j9
EOL
