#ifndef MAIN_PYBIND_SIMULATION_PARAMETERS_HPP
#define MAIN_PYBIND_SIMULATION_PARAMETERS_HPP

#ifdef BUILD_PYTHON_BINDINGS

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "../mcmc_simulation/header.hpp"
#include "../execution/executer.hpp"


/* Binds:
    - <system_name>EquilibriumTimeParameters
    - <system_name>CorrelationTimeParameters
    - <system_name>ExpectationValueParameters
    - <system_name>EquilibriumTime
    - <system_name>CorrelationTime
    - <system_name>ExpectationValue
*/

template<typename SystemParameters, typename System>
void init_simulation_parameters(py::module &m, std::string system_name)
{
    // Simulation Parameters

    // -> Equilibrium Time
    typedef mcmc::simulation::SimulationParameters<SystemParameters, mcmc::mode::EquilibriumTimeParameters> SystemEquilibriumTimeParameters;
    py::class_<SystemEquilibriumTimeParameters>(m, (system_name + "EquilibriumTimeParameters").c_str())
        .def_static("generate_simulation",
            py::overload_cast<
                SystemParameters&,
                mcmc::mode::EquilibriumTimeParameters&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::vector<double>&>(&SystemEquilibriumTimeParameters::generate_simulation),
            "systembase_parameters"_a, "execution_parameters"_a, "rel_data_path"_a,
            "running_parameter_kind"_a="None", "running_parameter"_a="None",
            "rp_intervals"_a=std::vector<double>{0.0})
        .def_readonly("rp_intervals", &SystemEquilibriumTimeParameters::rp_intervals);

    // -> Correlation Time
    typedef mcmc::simulation::SimulationParameters<SystemParameters, mcmc::mode::CorrelationTimeParameters> SystemCorrelationTimeParameters;
    py::class_<SystemCorrelationTimeParameters>(m, (system_name + "CorrelationTimeParameters").c_str())
        .def_static("generate_simulation",
            py::overload_cast<
                SystemParameters&,
                mcmc::mode::CorrelationTimeParameters&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::vector<double>&>(&SystemCorrelationTimeParameters::generate_simulation),
            "systembase_parameters"_a, "execution_parameters"_a, "rel_data_path"_a,
            "running_parameter_kind"_a="None", "running_parameter"_a="None",
            "rp_intervals"_a=std::vector<double>{0.0})
        .def_readonly("rp_intervals", &SystemCorrelationTimeParameters::rp_intervals);

    // -> Expectation Value
    typedef mcmc::simulation::SimulationParameters<SystemParameters, mcmc::mode::ExpectationValueParameters> SystemExpectationValueParameters;
    py::class_<SystemExpectationValueParameters>(m, (system_name + "ExpectationValueParameters").c_str())
        .def_static("generate_simulation",
            py::overload_cast<
                SystemParameters&,
                mcmc::mode::ExpectationValueParameters&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::vector<double>&>(&SystemExpectationValueParameters::generate_simulation),
            "systembase_parameters"_a, "execution_parameters"_a, "rel_data_path"_a,
            "running_parameter_kind"_a="None", "running_parameter"_a="None",
            "rp_intervals"_a=std::vector<double>{0.0})
        .def_readonly("rp_intervals", &SystemExpectationValueParameters::rp_intervals);
    
    // Simulation

    // -> Equilibrium Time
    typedef mcmc::simulation::Simulation<SystemParameters, mcmc::mode::EquilibriumTimeParameters> SystemEquilibriumTime;
    py::class_<SystemEquilibriumTime>(m, (system_name + "EquilibriumTime").c_str())
        .def(py::init<SystemEquilibriumTimeParameters&>())
        .def("run", &SystemEquilibriumTime::run)
        .def("evaluate", &SystemEquilibriumTime::evaluate, "rel_results_dir"_a, "sim_root_dir"_a);

    // -> Correlation Time
    typedef mcmc::simulation::Simulation<SystemParameters, mcmc::mode::CorrelationTimeParameters> SystemCorrelationTime;
    py::class_<SystemCorrelationTime>(m, (system_name + "CorrelationTime").c_str())
        .def(py::init<SystemCorrelationTimeParameters&>())
        .def("run", &SystemCorrelationTime::run)
        .def("evaluate", &SystemCorrelationTime::evaluate, "rel_results_dir"_a, "sim_root_dir"_a);

    // -> Expectation Value
    typedef mcmc::simulation::Simulation<SystemParameters, mcmc::mode::ExpectationValueParameters> SystemExpectationValue;
    py::class_<SystemExpectationValue>(m, (system_name + "ExpectationValue").c_str())
        .def(py::init<SystemExpectationValueParameters&>())
        .def("run", &SystemExpectationValue::run)
        .def("evaluate", &SystemExpectationValue::evaluate, "rel_results_dir"_a, "sim_root_dir"_a);
}

#endif // BUILD_PYTHON_BINDINGS

#endif // MAIN_PYBIND_SIMULATION_PARAMETERS_HPP
