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
    // Initialize system base class methods of ScalarTheory
    py::class_<mcmc::simulation::SystemBase<ScalarTheory>> scalar_theory_base(m, "ScalarTheorySystemBase");
    mcmc::pybind::init_systembase_methods<mcmc::simulation::SystemBase<ScalarTheory>>(scalar_theory_base);
    // Initialize further methods of ScalarTheory
    py::class_<ScalarTheory>(m, "ScalarTheorySystem", scalar_theory_base)
            .def(py::init<double, double, std::vector<int>, double, int, double>(),
                 "kappa"_a, "lambd"_a, "dimensions"_a=0.1,
                 "dt"_a, "n"_a, "m"_a=1.0);
}

PYBIND11_MODULE(scalartheorysimulation, m)
{
    // Inilialize simulation classes for mode simulations
    mcmc::pybind::init_simulation_modes<ScalarTheory, mcmc::measures::ReadableMeasure>(m, "ScalarTheory");
    // Initialize standalone version of ScalarTheory for custom simulations
    init_scalar_theory(m);

    m.doc() = "Python Bindings for ScalarTheory simulation";
}
