pybind11_add_module(mcmcsimulationlib_python mcmcsimulationlib_python.cpp)

# Project specific compliation options (used by config.h.in)
set(CLUSTER_MODE "local" CACHE STRING "Mode to run the cluster.")
# set(PYTHON_SCRIPTS_PATH "./../../python_scripts" CACHE STRING "Path to the potential python scripts evaluated by the python backend.")

configure_file(config.h.in ./../../python/config.h @ONLY)

# Added so that 
# find_library(ParamHelper NAMES libparamhelper.a PATHS ../build/ext/ParamHelper/src)
# message("ParamHelper = ${ParamHelper}")
# include_directories(../ext/ParamHelper/include/)

target_link_libraries(mcmcsimulationlib_python PUBLIC mcmcsimulationlib paramhelper::paramhelper)
set_target_properties(mcmcsimulationlib_python PROPERTIES OUTPUT_NAME mcmcsimulation)
