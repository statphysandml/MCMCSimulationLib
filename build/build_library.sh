#!/bin/bash

parent_dir="$(dirname -- "$(readlink -f -- "build_project.sh")")"
path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"

# Remove lib directory if already existing to avoid errors when changing parameters
if [ -d "${path_to_mcmc_simulation_lib}/lib" ]; then
  echo "Remove lib folder to avoid errors due to changing parameters."
  rm -r "${path_to_mcmc_simulation_lib}/lib"
fi

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

if test -f "${path_to_config}/config.sh"; then
  source "${path_to_config}/config.sh"
fi

# Generate generate_cmake_lists_txt_file
source generate_cmake_lists_txt_file.sh

# Generate config_h_in file in ../include/
include_path="../include/execution/"
source generate_config_h_in_file.sh

cd ../
mkdir -p lib
cd lib

cmake ..
make -j9

cd ../build/
