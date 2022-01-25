#include "../../include/command_line_support/cmd.hpp"

namespace mcmc {
    namespace from_file {

        // Project dependent parameters
        bool from_args_simulation = false;

        const std::map<std::string, ExecutionMode> mode_resolver = {
                {"expectation_value",      expectation_value},
                {"correlation_time",       correlation_time},
                {"equilibrium_time",       equilibrium_time}
        };

        void enable_from_args_simulation()
        {
            from_args_simulation = true;
        }
    }
}
