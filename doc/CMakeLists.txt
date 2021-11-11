{%- if cookiecutter._doxygen == "Yes" or cookiecutter._readthedocs == "Yes" -%}
find_package(_doxygen REQUIRED)
set(_doxygen_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/ext/*")
set(_doxygen_SHORT_NAMES YES)
{% if cookiecutter._readthedocs == "Yes" -%}
set(_doxygen_GENERATE_XML YES)
{%- endif %}
_doxygen_add_docs(_doxygen
  ${CMAKE_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT Building _doxygen documentation...
)
{% if cookiecutter._readthedocs == "Yes" -%}
add_custom_target(sphinx-doc
                  COMMAND
                  sphinx-build -b html
                  -Dbreathe_projects.{{ cookiecutter._project_slug }}="${CMAKE_CURRENT_BINARY_DIR}/xml"
                  -c ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_CURRENT_BINARY_DIR}/sphinx
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx..."
                  )
add_dependencies(sphinx-doc _doxygen)
{%- endif %}
{%- endif -%}