#ifndef RANDOMMT_HPP
#define RANDOMMT_HPP

#include <random>
#include <complex>

namespace mcmc {
    namespace util {

        static std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems
        static std::mt19937 gen(rd());

        void set_random_seed(uint32_t seed_val=0)
        {
            gen.seed(seed_val);
        }

        template<typename T>
        T sign(const T value) {
            if (value > 0)
                return T(1);
            else
                return T(-1.0);
        }

    }
}

#endif