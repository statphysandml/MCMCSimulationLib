#ifndef MAIN_SIMULATION_HEADER_HPP
#define MAIN_SIMULATION_HEADER_HPP

#ifdef RUN_WITH_PYTHON_BACKEND
#include <Python.h>
#endif

#include <mcmc_simulation/util/random.hpp>
#include <execution/executer.hpp>

#include "systembase_template.hpp"

namespace from_file_simulation {
    typedef SystemBaseTemplateParameters SystemBaseParams;
}

#endif //MAIN_SIMULATION_HEADER_HPP

