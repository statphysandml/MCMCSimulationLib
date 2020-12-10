# Verify if config.sh file has been generated
if ! test -f "config.sh"; then
  echo "config.sh file not in build directory. You can copy the config_template.sh file and adapt the parameters according to your system."
  exit 1
fi

# Build submodule
bash build_submodules.sh

# Build library
bash build_library.sh
