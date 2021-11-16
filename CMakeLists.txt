cmake_minimum_required(VERSION 3.15)

# Set a name and a version number for your project:
project(MCMCSimulationLib VERSION 0.0.1 LANGUAGES CXX)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compilation options
set(BUILD_PYTHON ON CACHE BOOL "Enable building of Python bindings")
set(BUILD_DOCS ON CACHE BOOL "Enable building of documentation")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ext/ParamHelper/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

configure_file(./include/execution/config.h.in ../include/execution/config.h @ONLY)

# Compilation options

# compile the external submodule
add_subdirectory(ext/ParamHelper)
include_directories(./ext/ParamHelper/include)

message("BUILD PYthon ${BUILD_PYTHON}")
if(BUILD_PYTHON)
    # Add Python bindings
    add_subdirectory(ext/pybind11)

    # Python virtual environment
    add_subdirectory(python)
else()
    # Add Python bindings for correct binding of pybind11 without usage of pybind11_modules
    add_subdirectory(ext/pybind11)
    set(target_link_libraries_python_appendix "pybind11::embed")
    # Or without pybind11:
    # set(Python3_FIND_VIRTUALENV "FIRST")
    # find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    # include_directories(${Python3_INCLUDE_DIRS})
    # set(target_link_libraries_python_appendix "${Python3_LIBRARIES}")
endif()

if(Python3_FOUND)
    option(PYTHON "Enable Python" ON)
    if(DEFINED VIRTUAL_ENV AND DEFINED CONDA_ACTIVATE_PATH)
        option(RUN_WITH_VIRTUAL_ENV "Enable que-script generation with virtual environment" ON)
    else()
        option(RUN_WITH_VIRTUAL_ENV "Disable que-script generation with virtual environment" OFF)
    endif()
else()
    # Use library without the integration of python code
    option(PYTHON "Disable Python" OFF)
endif()

# compile the library
add_subdirectory(src)


# compile the application
add_subdirectory(app)

# compile the tests
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(ext/Catch2)
  include(./ext/Catch2/contrib/Catch.cmake)
  add_subdirectory(tests)
endif()

if(BUILD_DOCS)
    # Add the documentation
    add_subdirectory(doc)
endif()


# Add an alias target for use if this project is included as a subproject in another project
add_library(mcmcsimulationlib::mcmcsimulationlib ALIAS mcmcsimulationlib)

# Install targets and configuration
install(
  TARGETS mcmcsimulationlib
  EXPORT mcmcsimulationlib-config
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT mcmcsimulationlib-config
  NAMESPACE mcmcsimulationlib::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mcmcsimulationlib
)

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
