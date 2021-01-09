cat >"${include_path}/config.h.in" <<EOL
#ifndef CONFIG_H_IN
#define CONFIG_H_IN

EOL
if [ -v path_to_python3 ]; then
cat >>"${include_path}/config.h.in" <<EOL
#cmakedefine VIRTUAL_ENV "@VIRTUAL_ENV@"
#cmakedefine CONDA_ACTIVATE_PATH "@CONDA_ACTIVATE_PATH@"
EOL
fi
cat >>"${include_path}/config.h.in" <<EOL

#endif // CONFIG_H_IN
EOL