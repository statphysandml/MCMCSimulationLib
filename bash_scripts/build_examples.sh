#!/bin/bash

# Examples

# MCMC Simulation lib
parent_dir="$(dirname -- "$(readlink -f -- "build_examples.sh")")"
path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"

path_to_base_lib=${path_to_mcmc_simulation_lib}

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

project_name="IsingModel"
project_path="../examples/"
project_path="$(cd "$project_path" && pwd -P)"
project_type="project"

# Build the project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_main_builder.sh"

# Compile the sample project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_compiling.sh"

# Build simulations

project_name="SaveAndLoad"
project_path="./simulations/SaveAndLoad/"
project_path="$(cd "$project_path" && pwd -P)"
project_type="simulation"
without_templates=true

# Build the project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_main_builder.sh"

# Compile the sample project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_compiling.sh"

# Navigate back to examples/
cd ../../

project_name="SimulateAndExecute"
project_path="./simulations/SimulateAndExecute/"
project_path="$(cd "$project_path" && pwd -P)"
project_type="simulation"
without_templates=true

# Build the project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_main_builder.sh"

# Compile the sample project
source "${path_to_mcmc_simulation_lib}/bash_scripts/generic_compiling.sh"
