#include <mcmc/mcmc_simulation/util/random.hpp>
#include <iostream>

namespace mcmc::util {
    std::random_device random::g_rd;

    std::mt19937 random::g_gen(random::g_rd());

    void random::set_random_seed(uint32_t seed_val)
    {
        std::cout << "Setting random seed" << std::endl;
        random::g_gen.seed(seed_val);
    }

    std::complex<double> sign(const std::complex<double> value) {
        return value / abs(value);
    }
}