# Config path
if [ -z ${path_to_config+x} ]; then
  path_to_config="$(dirname -- "$(readlink -f -- "build.sh")")"
fi

path_to_base_lib=${path_to_mcmc_simulation_lib}

# Verify if config.sh file has been generated
if ! test -f "${path_to_config}/config.sh"; then
  echo "config.sh file not in build directory. You can copy the config_template.sh file and adapt the parameters according to your system."
  exit 1
fi

# Build submodule
bash build_submodules.sh

# Build library
bash build_library.sh
