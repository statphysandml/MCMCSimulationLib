#!/bin/bash

# Build Debug
cd "$project_path/debug"

cmake ../cmake/. -DCMAKE_BUILD_TYPE=Debug -DCLUSTER_MODE=$cluster_mode
make -j9

# Build Release
cd ../release

cmake ../cmake/. -DCMAKE_BUILD_TYPE=Release -DCLUSTER_MODE=$cluster_mode
make -j9