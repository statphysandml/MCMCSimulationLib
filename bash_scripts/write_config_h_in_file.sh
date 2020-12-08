cat >"${include_path}/config.h.in" <<EOL
#ifndef ${project_name^^}_CONFIG_H_IN
#define ${project_name^^}_CONFIG_H_IN

#cmakedefine PROJECT_NAME "@PROJECT_NAME@"

#endif // ${project_name^^}_CONFIG_H_IN
EOL
