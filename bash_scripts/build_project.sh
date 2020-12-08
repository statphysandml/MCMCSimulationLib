#!/bin/bash

project_name="MCMCSimulationLibExamples"
project_path="../examples/"

if test -d "$project_path/include"; then
	echo "Main project already build. Only the cmake lists file is rebuild."

	mkdir -p "$project_path/cmake"
	mkdir -p "$project_path/debug"
	mkdir -p "$project_path/release"

  # Submodules
  path_to_param_helper="../../external_submodules/ParamHelper/"
  path_to_mcmc_simulation_lib="../../"

  source ../build/config.sh

	# Generate config.h.in file
	include_path="../examples/include/"
	source write_config_h_in_file.sh

	# Generate CMakeLists.txt file
	source write_cmake_lists_file_main_project.sh

	echo $project_path/cmake/CMakeLists.txt
else
	# Generate folders for main project
	mkdir "$project_path/include"
	mkdir "$project_path/src"
	mkdir "$project_path/jupyter_notebooks"

	include_path="$project_path/include/"
	src_path="$project_path/src/"

	source "${path_to_lattice_model_implementations}bash_scripts/build_structure.sh"
fi