# Config path
if [ -z ${path_to_config+x} ]; then # Enables using the config files from a different path
  path_to_config="$(dirname -- "$(readlink -f -- "build.sh")")"
fi

# Verify if config.sh file has been generated
if ! test -f "${path_to_config}/config.sh"; then
  echo "Remark: No config.sh file found in build directory. You won't be able to use the python functionalities of the library without providing a virtual environment and the necessary parameters in config.sh. The config_template.sh file contains all important parameters. You can copy it, rename it to config.sh and adapt it to your system."
fi

# Build submodule
bash build_submodules.sh

# Build library
source build_library.sh
