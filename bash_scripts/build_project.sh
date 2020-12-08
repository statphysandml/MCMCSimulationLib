#!/bin/bash

# Project name
read -p "Enter main project name: " project_name

if test -z $project_name; then
	echo "Project name cannot be empty."
	exit 1
fi

# Project Path
read -p "Enter path to project (absolute or relative): " project_path

if test -z $project_path; then
	echo "Path to project cannot be empty."
	exit 1
fi

mkdir -p "$project_path/"

source generate_project.sh