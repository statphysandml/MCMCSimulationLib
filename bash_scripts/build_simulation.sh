#!/bin/bash

# MCMC Simulation lib
if [ -z ${path_to_mcmc_simulation_lib+x} ]; then
  parent_dir="$(dirname -- "$(readlink -f -- "build_project.sh")")"
  path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"
fi

path_to_base_lib=${path_to_mcmc_simulation_lib}

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

# Determine project_name, project_path and project_type
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_simulation_builder.sh"

# Build the project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_main_builder.sh"

cp "${path_to_mcmc_simulation_lib}/templates/systembase_template.hpp" ${include_path}
cp "${path_to_mcmc_simulation_lib}/templates/plain_systembase_template.hpp" ${include_path}

# Compile the sample project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_compiling.sh"
