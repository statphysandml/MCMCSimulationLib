#include "../../../include/mcmc_simulation/util/random.hpp"

namespace mcmc {
    namespace util {
        
        std::random_device rd;

        std::mt19937 gen(rd());

        void set_random_seed(uint32_t seed_val)
        {
            gen.seed(seed_val);
        }

        std::complex<double> sign(const std::complex<double> value) {
            return value / abs(value);
        }

    }
}