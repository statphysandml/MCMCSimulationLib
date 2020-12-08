cat >"${include_path}/config.h.in" <<EOL
#ifndef CONFIG_H_IN
#define CONFIG_H_IN

#cmakedefine CLUSTER_MODE "@CLUSTER_MODE@"
EOL
if [ -v path_to_python3 ]; then
cat >>"${include_path}/config.h.in" <<EOL
#cmakedefine PYTHON_SCRIPTS_PATH "@PYTHON_SCRIPTS_PATH@"
#cmakedefine CONDA_ACTIVATE_PATH "@CONDA_ACTIVATE_PATH@"
#cmakedefine VIRTUAL_ENV "@VIRTUAL_ENV@"
EOL
fi
cat >>"${include_path}/config.h.in" <<EOL

#endif // CONFIG_H_IN
EOL
