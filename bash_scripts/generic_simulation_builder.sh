#!/bin/bash

# If condition enables building simulations from other bash scripts as long as project_name is defined
if test -z $project_name; then
    # Simulation name
    read -p "Enter simulation name: " project_name
fi
    
if test -z $project_name; then
	echo "Simluation name cannot be empty."
	exit 1
fi

# Project Path
# Different path not working at the moment because of a potential wrong pysthon_scripts
# read -p "Enter path to simulation base directory (absolute or relative) (default ../simulations/): " simulation_dir

if test -z $simulation_dir; then
  simulation_dir="../simulations/"
fi

mkdir -p "$simulation_dir/"

project_path="$simulation_dir/$project_name"
mkdir -p "$project_path/"
project_path="$(cd "$project_path" && pwd -P)"
echo "Absolute simulation path: ${project_path}"

project_type="simulation"
