#!/bin/bash

# Submodules
path_to_param_helper="./external_submodules/ParamHelper/"

source config.sh

source generate_cmake_lists_txt_file.sh

cd ../
mkdir -p lib
cd lib

cmake ..
make -j9

cd ../build/
