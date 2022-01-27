add_library(mcmcsimulationlib STATIC
  mcmc_simulation/simulation.cpp
  mcmc_simulation/util/random.cpp
  mcmc_simulation/util/python_integration.cpp
  mcmc_simulation/sampler/gaussian_sampler.cpp
  mcmc_simulation/sampler/hat_function_sampler.cpp
  mcmc_simulation/sampler/uniform_sampler.cpp
  cluster_support/cluster_integration.cpp
  cluster_support/gpu_cluster_integration.cpp
  cluster_support/cpu_cluster_integration.cpp
  cluster_support/virtual_env_integration.cpp
  command_line_support/cmdint.cpp)
  

if (RUN_WITH_PYTHON_BACKEND)
  target_compile_definitions(mcmcsimulationlib PUBLIC PYTHON_BACKEND)
  target_link_libraries(mcmcsimulationlib PRIVATE ${target_link_libraries_python_appendix})
endif()

target_include_directories(mcmcsimulationlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
