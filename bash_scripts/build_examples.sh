#!/bin/bash

# Project name
read -p "Enter main project name: " project_name

if test -z $project_name; then
	echo "Project name cannot be empty."
	exit 1
fi

project_path="../examples/"

if test -d "$project_path/include"; then
	echo "Main project already build. Only the cmake lists file is rebuild."

	mkdir -p "$project_path/cmake"
	mkdir -p "$project_path/debug"
	mkdir -p "$project_path/release"
	# Generate CMakeLists.txt file
	source "${path_to_lattice_model_implementations}bash_scripts/write_cmake_lists_file_main_project.sh"
else
	# Generate folders for main project
	mkdir "$project_path/include"
	mkdir "$project_path/src"
	mkdir "$project_path/jupyter_notebooks"

	include_path="$project_path/include/"
	src_path="$project_path/src/"

	source "${path_to_lattice_model_implementations}bash_scripts/build_structure.sh"
fi


#!/bin/bash

project_name="MCMCSimulationLibExamples"
project_path="../examples/"

mkdir -p "$project_path/cmake"
mkdir -p "$project_path/debug"
mkdir -p "$project_path/release"
mkdir -p "$project_path/jupyter_notebooks"

# Generate CMakeLists.txt file
source "${path_to_lattice_model_implementations}bash_scripts/write_cmake_lists_file_main_project.sh"

# Generate config_h_in file
source "${path_to_lattice_model_implementations}bash_scripts/write_config_h_in_file.sh"
