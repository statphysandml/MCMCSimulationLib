#!/bin/bash

parent_dir="$(dirname -- "$(readlink -f -- "build_project.sh")")"
path_to_mcmc_simulation_lib="$(dirname "$parent_dir")"

# Submodules
path_to_param_helper="${path_to_mcmc_simulation_lib}/external_submodules/ParamHelper/"

source "${path_to_config}/config.sh"

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
