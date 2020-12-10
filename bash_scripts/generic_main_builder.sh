#!/bin/bash

# Generate folder structure
mkdir -p "$project_path/debug"
mkdir -p "$project_path/release"
mkdir -p "$project_path/cmake"

if [ "$project_type" = "project" ]; then
  include_path="$project_path/include"
  src_path="$project_path/src"
else
  include_path="$project_path"
  src_path="$project_path"
fi

# Will be generated later by the C++ program - doesn't work so far
mkdir -p "$project_path/configs"
mkdir -p "$project_path/data"
mkdir -p "$project_path/cpu_cluster_runs"

source "${path_to_base_lib}/build/config.sh"

if test -f "$src_path/main.cpp"; then
	echo "Main project already build. Only the cmake lists file is rebuild."

	# Generate config.h.in file
	source "${path_to_base_lib}/bash_scripts/write_config_h_in_file.sh"

	# Generate CMakeLists.txt file
	source "${path_to_base_lib}/bash_scripts/write_cmake_lists_file_main_project.sh"
else

	if [ "$project_type" = "project" ]; then
	  # Generate folders for main project
    mkdir "$include_path"
    mkdir "$src_path"
    mkdir "$project_path/jupyter_notebooks"
  fi

  # Generate config_h_in file
  source "${path_to_base_lib}/bash_scripts/write_config_h_in_file.sh"

  # Generate CMakeLists.txt file
  source "${path_to_base_lib}/bash_scripts/write_cmake_lists_file_main_project.sh"

  # Generate main.cpp file
  source "${path_to_base_lib}/bash_scripts/write_main_cpp_file.sh"

  # Generate simulation_header.hpp file
  source "${path_to_base_lib}/bash_scripts/write_simulation_header_hpp_file.sh"
fi