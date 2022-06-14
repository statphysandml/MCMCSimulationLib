#ifndef MAIN_PYBIND_SIMULATION_PARAMETERS_HPP
#define MAIN_PYBIND_SIMULATION_PARAMETERS_HPP

#ifdef PYTHON_BACKEND

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include <mcmc/mcmc_simulation/header.hpp>
#include <mcmc/modes/mode_header.hpp>


/* Binds:
    - system
    - <system_name>EquilibriumTime
    - <system_name>CorrelationTime
    - <system_name>ExpectationValue
*/

namespace mcmc::pybind {

    template<typename System>
    void init_systembase_methods(py::class_<System> &system)
    {
        system.def("update", &System::update, "measure_interval"_a=1)
            .def("init", &System::init, "starting_mode"_a)
            .def("measure", &System::measure)
            .def("measure_names", &System::measure_names)
            .def("set_measures", &System::set_measures, "measures"_a)
            .def("size", &System::size)
            .def("__getitem__", static_cast<const decltype(std::declval<System const&>().operator[](0)) (System::*)(int) const>(&System::operator[]))
            .def("__setitem__", static_cast<void (System::*)(int, decltype(std::declval<System&>().operator[](0)))>(&System::set_system_elem))
            .def_property("system_repr",
                static_cast<const decltype(std::declval<System const&>().operator()()) (System::*)() const>(&System::operator()),
                static_cast<void (System::*)(decltype(std::declval<System&>().operator()()))>(&System::set_system)
            );
    }

    template<typename System, typename MeasureProcessor>
    void init_simulation_modes(py::module &m, std::string system_name)
    {
        // Simulations

        // Equilibrium Time
        typedef mcmc::simulation::Simulation<System, mcmc::mode::EquilibriumTime, MeasureProcessor> SystemEquilibriumTime;
        py::class_<SystemEquilibriumTime>(m, (system_name + "EquilibriumTime").c_str())
            .def_static("generate_simulation",
                py::overload_cast<
                    System&,
                    mcmc::mode::EquilibriumTime&,
                    MeasureProcessor&,
                    const std::string&,
                    const std::string&,
                    const std::vector<double>&>(&SystemEquilibriumTime::generate_simulation),
                "systembase_parameters"_a, "execution_parameters"_a, "measure_parameters"_a,
                "running_parameter_kind"_a="None", "running_parameter"_a="None",
                "rp_intervals"_a=std::vector<double>{0.0})
            .def_readonly("rp_intervals", &SystemEquilibriumTime::rp_intervals_)
            .def("run", &SystemEquilibriumTime::run)
            .def("eval", &SystemEquilibriumTime::eval, "rel_results_dir"_a);

        // Correlation Time
        typedef mcmc::simulation::Simulation<System, mcmc::mode::CorrelationTime, MeasureProcessor> SystemCorrelationTime;
        py::class_<SystemCorrelationTime>(m, (system_name + "CorrelationTime").c_str())
            .def_static("generate_simulation",
                py::overload_cast<
                    System&,
                    mcmc::mode::CorrelationTime&,
                    MeasureProcessor&,
                    const std::string&,
                    const std::string&,
                    const std::vector<double>&>(&SystemCorrelationTime::generate_simulation),
                "systembase_parameters"_a, "execution_parameters"_a, "measure_parameters"_a,
                "running_parameter_kind"_a="None", "running_parameter"_a="None",
                "rp_intervals"_a=std::vector<double>{0.0})
            .def_readonly("rp_intervals", &SystemCorrelationTime::rp_intervals_)
            .def("run", &SystemCorrelationTime::run)
            .def("eval", &SystemCorrelationTime::eval, "rel_results_dir"_a);

        // Expectation Value
        typedef mcmc::simulation::Simulation<System, mcmc::mode::ExpectationValue, MeasureProcessor> SystemExpectationValue;
        py::class_<SystemExpectationValue>(m, (system_name + "ExpectationValue").c_str())
            .def_static("generate_simulation",
                py::overload_cast<
                    System&,
                    mcmc::mode::ExpectationValue&,
                    MeasureProcessor&,
                    const std::string&,
                    const std::string&,
                    const std::vector<double>&>(&SystemExpectationValue::generate_simulation),
                "systembase_parameters"_a, "execution_parameters"_a, "measure_parameters"_a,
                "running_parameter_kind"_a="None", "running_parameter"_a="None",
                "rp_intervals"_a=std::vector<double>{0.0})
            .def_readonly("rp_intervals", &SystemExpectationValue::rp_intervals_)
            .def("run", &SystemExpectationValue::run)
            .def("eval", &SystemExpectationValue::eval, "rel_results_dir"_a);
    }
}
#endif // PYTHON_BACKEND

#endif // MAIN_PYBIND_SIMULATION_PARAMETERS_HPP
