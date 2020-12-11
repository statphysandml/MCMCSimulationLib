#!/bin/bash

# Simualtion name
read -p "Enter simulation name: " project_name

if test -z $project_name; then
	echo "Simluation name cannot be empty."
	exit 1
fi

# Project Path
read -p "Enter path to simulation base directory (absolute or relative) (default ../simulations/): " simulation_dir

if test -z $simulation_dir; then
  simulation_dir="../simulations/"
fi

mkdir -p "$simulation_dir/"

project_path="$simulation_dir/$project_name"
mkdir -p "$project_path/"

project_type="simulation"