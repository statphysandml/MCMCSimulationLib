find_package(Doxygen REQUIRED)
set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/ext/*; ${CMAKE_SOURCE_DIR}/application_wrapper/*; ${CMAKE_SOURCE_DIR}/examples/*; ${CMAKE_SOURCE_DIR}/install/*")
set(DOXYGEN_SHORT_NAMES YES)
set(DOXYGEN_GENERATE_XML YES)
doxygen_add_docs(mcmcsimulationlib_doxygen
  ${CMAKE_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT Building doxygen documentation...
)
add_custom_target(mcmcsimulationlib-sphinx-doc
                  COMMAND
                  sphinx-build -b html
                  -Dbreathe_projects.MCMCSimulationLib="${CMAKE_CURRENT_BINARY_DIR}/xml"
                  -c ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_CURRENT_SOURCE_DIR}
                  ${CMAKE_CURRENT_BINARY_DIR}/sphinx
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx..."
                  )

add_dependencies(mcmcsimulationlib-sphinx-doc mcmcsimulationlib_doxygen)