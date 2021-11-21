add_executable(mcmcsimulationlib_tests tests.cpp MCMCSimulationLib_t.cpp)
target_link_libraries(mcmcsimulationlib_tests PUBLIC mcmcsimulationlib Catch2::Catch2)

# allow user to run tests with `make test` or `ctest`
catch_discover_tests(mcmcsimulationlib_tests)
