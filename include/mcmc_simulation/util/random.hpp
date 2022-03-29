#ifndef RANDOMMT_HPP
#define RANDOMMT_HPP

/** \file random.hpp
 * Definition of the random generator used throughout the entire project and further useful functions.
 * 
 */

#include <random>
#include <complex>

namespace mcmc {
    namespace util {
        /**
         * Random device engine, usually based on /dev/random on UNIX-like systems
         */
        extern std::random_device rd;
        /** 
         * Mersenne Twister pseudo-random generator used throughout the entire project
         */
        extern std::mt19937 gen;

        /** @brief Sets the random seet for mcmc::util::gen used throughout the entire project
         *
         * @param seed_val Random seed
         * @returns None
         */
        void set_random_seed(uint32_t seed_val=0);
        
        /** @brief Sign function
         *
         * @param value Value
         * @returns 1 if value > 0 and -1 if value <= 0
         */
        template<typename T>
        T sign(const T value) {
            if (value > 0)
                return T(1);
            else
                return T(-1);
        }
    }
}

#endif