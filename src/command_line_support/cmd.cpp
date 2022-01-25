#include "../../include/command_line_support/cmd.hpp"

namespace mcmc {
    namespace cmdint {

        // Project dependent parameters
        bool from_args_simulation = false;

        const std::map<std::string, mcmc::cmdint::ExecutionMode> mode_resolver = {
                {"expectation_value",      mcmc::cmdint::expectation_value},
                {"correlation_time",       mcmc::cmdint::correlation_time},
                {"equilibrium_time",       mcmc::cmdint::equilibrium_time}
        };

        void enable_from_args_simulation()
        {
            from_args_simulation = true;
        }
    }
}
