#!/bin/bash

# MCMC Simulation lib
parent_dir="$(dirname -- "$(readlink -f -- "generate_project.sh")")"
path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"
echo $parent_dir
echo $path_to_mcmc_simulation_lib

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

# Generate folder structure
mkdir -p "$project_path/cmake"
mkdir -p "$project_path/debug"
mkdir -p "$project_path/release"

mkdir -p "$project_path/configs"
mkdir -p "$project_path/data"
mkdir -p "$project_path/cpu_cluster_runs"

include_path="$project_path/include/"
src_path="$project_path/src/"

source ../build/config.sh

if test -d "$project_path/include"; then
	echo "Main project already build. Only the cmake lists file is rebuild."

	# Generate config.h.in file
	source "${path_to_mcmc_simulation_lib}/bash_scripts/write_config_h_in_file.sh"

	# Generate CMakeLists.txt file
	source "${path_to_mcmc_simulation_lib}/bash_scripts/write_cmake_lists_file_main_project.sh"
else
	# Generate folders for main project
	mkdir "$project_path/include"
	mkdir "$project_path/src"
	mkdir "$project_path/jupyter_notebooks"

  cp "${path_to_mcmc_simulation_lib}/templates/systembase_measures_template.hpp" ${include_path}
  cp "${path_to_mcmc_simulation_lib}/templates/systembase_template.hpp" ${include_path}

  # Generate config_h_in file
  source "${path_to_mcmc_simulation_lib}/bash_scripts/write_config_h_in_file.sh"

  # Generate CMakeLists.txt file
  source "${path_to_mcmc_simulation_lib}/bash_scripts/write_cmake_lists_file_main_project.sh"

  # Generate main.cpp file
  source "${path_to_mcmc_simulation_lib}/bash_scripts/write_main_cpp_file.sh"

  # Generate simulation_header.hpp file
  source "${path_to_mcmc_simulation_lib}/bash_scripts/write_simulation_header_hpp_file.sh"
fi

# Build Debug
cd "$project_path/debug"

cmake ../cmake/. -DCMAKE_BUILD_TYPE=Debug -DCLUSTER_MODE=$cluster_mode
make -j9

# Build Release
cd ../release

cmake ../cmake/. -DCMAKE_BUILD_TYPE=Release -DCLUSTER_MODE=$cluster_mode
make -j9

