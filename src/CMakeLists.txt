add_library(mcmcsimulationlib STATIC
  mcmc_simulation/simulation.cpp
  mcmc_simulation/util/random.cpp
  mcmc_simulation/sampler/gaussian_sampler.cpp
  mcmc_simulation/sampler/hat_function_sampler.cpp
  mcmc_simulation/sampler/uniform_sampler.cpp
  execution/executer.cpp)

if (PYTHON)
  target_compile_definitions(mcmcsimulationlib PUBLIC -D PYTHON)
endif()
target_link_libraries(mcmcsimulationlib ${ParamHelper} ${PYTHON_LIBRARIES})

target_include_directories(mcmcsimulationlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
