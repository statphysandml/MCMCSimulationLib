#include <mcmc/command_line_support/cmdint.hpp>

namespace mcmc::cmdint {
    const std::map<std::string, mcmc::cmdint::ExecutionMode> g_mode_resolver = {
            {"expectation_value",      mcmc::cmdint::expectation_value},
            {"correlation_time",       mcmc::cmdint::correlation_time},
            {"equilibrium_time",       mcmc::cmdint::equilibrium_time}
    };
}
