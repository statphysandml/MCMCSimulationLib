pybind11_add_module({{ cookiecutter.project_slug.replace("-", "") }}_python {{ cookiecutter.project_slug.replace("-", "")|slugify }}_python.cpp)

if(RUN_WITH_PYTHON_BACKEND)
 target_compile_definitions({{ cookiecutter.project_slug.replace("-", "") }}_python PUBLIC PYTHON_BACKEND)
endif()

target_link_libraries({{ cookiecutter.project_slug.replace("-", "") }}_python PUBLIC mcmcsimulationlib::mcmcsimulationlib paramhelper::paramhelper)
set_target_properties({{ cookiecutter.project_slug.replace("-", "") }}_python PROPERTIES OUTPUT_NAME {{ cookiecutter.project_slug.replace("-", "") }}simulation)