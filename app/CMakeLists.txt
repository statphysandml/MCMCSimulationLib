add_executable(MCMCSimulationLib_examples MCMCSimulationLib_app.cpp)
if (PYTHON)
    target_link_libraries(MCMCSimulationLib_examples PRIVATE mcmcsimulationlib ${target_link_libraries_python_appendix})
else()
    target_link_libraries(MCMCSimulationLib_examples PRIVATE mcmcsimulationlib)
endif()

