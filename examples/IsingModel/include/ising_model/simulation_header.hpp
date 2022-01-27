#ifndef MAIN_SIMULATION_HEADER_HPP
#define MAIN_SIMULATION_HEADER_HPP

#ifdef PYTHON_BACKEND
#include <Python.h>
#endif
#include <mcmc_simulation/util/random.hpp>

#include "system.hpp"

namespace from_file_simulation {
    typedef SystemParameters SystemBaseParams;
}

#endif //MAIN_SIMULATION_HEADER_HPP

