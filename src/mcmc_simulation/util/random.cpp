//
// Created by lukas on 31.07.20.
//

#include "../../../include/mcmc_simulation/util/random.hpp"

namespace mcmc {
    namespace util {

        std::complex<double> sign(const std::complex<double> value) {
            return value / abs(value);
        }

    }
}