#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>

namespace py = pybind11;
using namespace pybind11::literals;

#include <mcmc_pybind/pybind_simulation_parameters.hpp>


#include "../include/scalar_theory_gpu/scalar_theory_gpu.hpp"

void init_scalar_theory(py::module &m)
{
    // Initialize system base class methods of ScalarTheoryGPU
    py::class_<mcmc::simulation::SystemBase<ScalarTheoryGPU>> scalar_theory_base(m, "ScalarTheoryGPUSystemBase");
    mcmc::pybind::init_systembase_methods<mcmc::simulation::SystemBase<ScalarTheoryGPU>>(scalar_theory_base);
    // Initialize further methods of ScalarTheoryGPU
    py::class_<ScalarTheoryGPU>(m, "ScalarTheoryGPUSystem", scalar_theory_base)
            .def(py::init<double, double, std::vector<int>, double, int, double>(),
                 "kappa"_a, "lambd"_a, "dimensions"_a=0.1,
                 "dt"_a, "n"_a, "m"_a=1.0);
}

PYBIND11_MODULE(scalartheorygpusimulation, m)
{
    // Inilialize simulation classes for mode simulations
    mcmc::pybind::init_simulation_modes<ScalarTheoryGPU, mcmc::measures::ReadableMeasure>(m, "ScalarTheoryGPU");
    // Initialize standalone version of ScalarTheoryGPU for custom simulations
    init_scalar_theory(m);

    m.doc() = "Python Bindings for ScalarTheoryGPU simulation";
}

