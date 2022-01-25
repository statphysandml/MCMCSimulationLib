#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>

#include "config.h"

// #include "MCMCSimulationLib/MCMCSimulationLib.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

#include <mcmc_simulation/header.hpp>
#include <execution/executer.hpp>
#include <execution/python_integration.hpp>


void init_functions(py::module &m)
{
    m.def("init_mcmc_python_binding", &mcmc::util::initialize_python, "python_modules_path"_a, "fma_develop"_a=false, "executing_from_python"_a=true);
}


void init_execution_modes(py::module &m)
{
    // Equilibrium Time
    py::class_<mcmc::mode::EquilibriumTimeParameters>(m, "EquilibriumTimeParameters")
        .def(py::init<uint, uint, double, double, std::string>(),
            "sample_size"_a=100, "number_of_steps"_a=1000, "confidence_range"_a=0.1,
            "confidence_window"_a=10, "measure"_a="Mean");

    // Correlation Time
    py::class_<mcmc::mode::CorrelationTimeParameters>(m, "CorrelationTimeParameters")
        .def(py::init<uint, uint, uint, std::string, std::string>(),
            "minimum_sample_size"_a=100, "maximum_correlation_time"_a=1000, "start_measuring"_a=0,
            "measure"_a="Mean", "starting_mode"_a="hot")
        .def(py::init<uint, uint, std::string, std::string, std::string>(),
            "minimum_sample_size"_a=100, "maximum_correlation_time"_a=1000, "equilibrium_time_rel_results_path"_a,
            "measure"_a="Mean", "starting_mode"_a="hot");

    // Expectation Value
    py::class_<mcmc::mode::ExpectationValueParameters>(m, "ExpectationValueParameters")
        .def(py::init<uint, uint, uint, std::vector<std::string>, std::vector<std::string>, uint, std::string>(),
            "measure_interval"_a=1, "number_of_measurements"_a=1000, "start_measuring"_a=0,
            "measures"_a=std::vector<std::string>{}, "post_measures"_a=std::vector<std::string>{},
            "n_means_bootstrap"_a=0, "starting_mode"_a="hot")
        .def(py::init<std::string, uint, uint, std::vector<std::string>, std::vector<std::string>, uint, std::string>(),
            "correlation_time_rel_results_path"_a, "number_of_measurements"_a=1000, "start_measuring"_a=0,
            "measures"_a=std::vector<std::string>{}, "post_measures"_a=std::vector<std::string>{},
            "n_means_bootstrap"_a=0, "starting_mode"_a="hot")
        .def(py::init<uint, uint, std::string, std::vector<std::string>, std::vector<std::string>, uint, std::string>(),
            "measure_interval"_a=1, "number_of_measurements"_a=1000, "equilibrium_time_rel_results_path"_a,
            "measures"_a=std::vector<std::string>{}, "post_measures"_a=std::vector<std::string>{},
            "n_means_bootstrap"_a=0, "starting_mode"_a="hot")
        .def(py::init<std::string, uint, std::string, std::vector<std::string>, std::vector<std::string>, uint, std::string>(),
            "correlation_time_rel_results_path"_a, "number_of_measurements"_a=1000, "equilibrium_time_rel_results_path"_a,
            "measures"_a=std::vector<std::string>{}, "post_measures"_a=std::vector<std::string>{},
            "n_means_bootstrap"_a=0, "starting_mode"_a="hot");
}

PYBIND11_MODULE(mcmcsimulation, m)
{
    init_functions(m);
    init_execution_modes(m);

    m.doc() = "Python Bindings for MCMCSimulationLib";
}

