# Config path
if [ -z ${path_to_config+x} ]; then
  path_to_config="$(dirname -- "$(readlink -f -- "build.sh")")"
fi

# Verify if config.sh file has been generated
if ! test -f "${path_to_config}/config.sh"; then
  echo "config.sh file not in build directory. You can copy the config_template.sh file and adapt the parameters according to your system."
  exit 1
fi

# Build submodule
bash build_submodules.sh

# Build library
source build_library.sh
