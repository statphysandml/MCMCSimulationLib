#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>

namespace py = pybind11;
using namespace pybind11::literals;

#include <mcmc_pybind/pybind_simulation_parameters.hpp>

#include "../include/scalar_theory/scalar_theory.hpp"

void init_scalar_theory(py::module &m)
{
    py::class_<ScalarTheoryParameters>(m, "ScalarTheoryParameters")
        .def(py::init<double, double, std::vector<int>, double, int, double>(),
            "kappa"_a, "lambd"_a, "dimensions"_a=0.1,
            "dt"_a, "n"_a, "m"_a=1.0);
    
    py::class_<ScalarTheory>(m, "ScalarTheory")
        .def(py::init<ScalarTheoryParameters&>(), "stsp");
}

PYBIND11_MODULE(scalartheorysimulation, m)
{
    init_scalar_theory(m);
    init_simulation_parameters<ScalarTheoryParameters, ScalarTheory>(m, "ScalarTheory");

    m.doc() = "Python Bindings for ScalarTheory simulation";
}

