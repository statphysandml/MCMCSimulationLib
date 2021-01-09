#!/bin/bash

# For an entire project

# MCMC Simulation lib
parent_dir="$(dirname -- "$(readlink -f -- "build_project.sh")")"
path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"

path_to_base_lib=${path_to_mcmc_simulation_lib}

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

# Determine project_name, project_path and project_name
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_project_builder.sh"

# Build the project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_main_builder.sh"

cp "${path_to_mcmc_simulation_lib}/templates/systembase_template.hpp" ${include_path}
cp "${path_to_mcmc_simulation_lib}/templates/plain_systembase_template.hpp" ${include_path}

# Compile the sample project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_compiling.sh"

# Add bash_script for possibility to generate simulations
mkdir -p "$project_path/bash_scripts/"
source "${path_to_mcmc_simulation_lib}/bash_scripts/write_project_related_build_simulation_sh_file.sh"