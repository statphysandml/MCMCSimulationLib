#!/bin/bash

# Submodules
path_to_param_helper="./external_submodules/ParamHelper/"
path_to_mcmc_simulation_lib="./"

source config.sh

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
