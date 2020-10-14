#!/bin/bash

source config.sh

source generate_cmake_lists_txt_file.sh

cd ../
mkdir lib
cd lib

cmake ..
make -j9
