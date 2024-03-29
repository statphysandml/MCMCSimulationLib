#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>

namespace py = pybind11;
using namespace pybind11::literals;

#include <mcmc/mcmc_pybind/pybind_simulation_parameters.hpp>


#include "../include/{{ cookiecutter.project_slug }}/{{ cookiecutter.project_slug }}.hpp"

void init_{{ cookiecutter.project_slug }}(py::module &m)
{
    // Initialize system base class methods of {{ cookiecutter.project_name }}
    py::class_<mcmc::simulation::SystemBase<{{ cookiecutter.project_name }}>> {{ cookiecutter.project_slug }}_base(m, "{{ cookiecutter.project_name }}SystemBase");
    mcmc::pybind::init_systembase_methods<mcmc::simulation::SystemBase<{{ cookiecutter.project_name }}>>({{ cookiecutter.project_slug }}_base);
    // Initialize further methods of {{ cookiecutter.project_name }}
    py::class_<{{ cookiecutter.project_name }}>(m, "{{ cookiecutter.project_name }}System", {{ cookiecutter.project_slug }}_base)
            .def(py::init<std::vector<double>, double, double>(), "mu"_a, "sigma"_a, "dt"_a);
}

PYBIND11_MODULE({{ cookiecutter.project_slug }}simulation, m)
{
    // Inilialize simulation classes for mode simulations
    mcmc::pybind::init_simulation_modes<{{ cookiecutter.project_name }}, mcmc::measures::ReadableMeasure>(m, "{{ cookiecutter.project_name }}");
    // Initialize standalone version of {{ cookiecutter.project_name }} for custom simulations
    init_{{ cookiecutter.project_slug }}(m);

    m.doc() = "Python Bindings for {{ cookiecutter.project_name }} simulation";
}
