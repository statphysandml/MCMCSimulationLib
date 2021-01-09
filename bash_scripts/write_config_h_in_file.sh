cat >"${include_path}/config.h.in" <<EOL
#ifndef ${project_name^^}_CONFIG_H_IN
#define ${project_name^^}_CONFIG_H_IN

#cmakedefine PROJECT_NAME "@PROJECT_NAME@"
#cmakedefine CLUSTER_MODE "@CLUSTER_MODE@"
#cmakedefine PYTHON_SCRIPTS_PATH "@PYTHON_SCRIPTS_PATH@"

#endif // ${project_name^^}_CONFIG_H_IN
EOL
