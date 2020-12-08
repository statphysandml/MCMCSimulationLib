# Build submodule

git submodule update --init

cd external_submodules/ParamHelper/build
bash build_submodules.sh

cd ../../../

# Build library

# Copy config_tempalte rm to config.sh and adapt paths

cd build/
bash build_library.sh

cd ../


