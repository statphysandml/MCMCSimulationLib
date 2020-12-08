cat >"${include_path}/simulation_header.hpp" <<EOL
#ifndef MAIN_SIMULATION_HEADER_HPP
#define MAIN_SIMULATION_HEADER_HPP

#ifdef PYTHON
#include <Python.h>
#endif
#include "mcmc_simulation/util/random.hpp"

#include "execution/executer.hpp"

// For systembases with custom measures
#include "systembase_template.hpp"

// For systembases with the possible usage of predefined common measures
#include "systembase_measures_template.hpp"

namespace from_file_simulation {
    typedef SystemBaseTemplateParameters SystemBaseParams;
}

#endif //MAIN_SIMULATION_HEADER_HPP

EOL