#!/bin/bash

# Project name
read -p "Enter main project name: " project_name

if test -z $project_name; then
	echo "Project name cannot be empty."
	exit 1
fi

# Project Path
read -p "Enter the path for the project (absolute or relative): " project_base_dir

if test -z $project_base_dir; then
	echo "Path to project cannot be empty."
	exit 1
fi

project_path="$project_base_dir/$project_name"
mkdir -p "$project_path/"
project_path="$(cd "$project_path" && pwd -P)"
echo "Absolute Project path: ${project_path}"

project_type="project"