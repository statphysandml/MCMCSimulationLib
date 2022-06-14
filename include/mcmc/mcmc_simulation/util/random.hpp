#ifndef RANDOMMT_HPP
#define RANDOMMT_HPP

/** \file random.hpp
 * Definition of the random generator used throughout the entire project and further useful functions.
 * 
 */

#include <random>
#include <complex>

namespace mcmc::util {

    struct random
    {
        /**
         * Random device engine, usually based on /dev/random on UNIX-like
         * systems.
         */
        static std::random_device g_rd;
        /** 
         * Mersenne Twister pseudo-random generator used throughout the entire
         * project.
         */
        static std::mt19937 g_gen;

        /** @brief Sets the random seed for ``mcmc::util::random::g_gen`` used
         * throughout the entire project.
         *
         * @param seed_val Random seed
         * @returns None
         */
        static void set_random_seed(uint32_t seed_val=0);
    };
    
    /** @brief Sign function.
     *
     * @param value Value
     * @returns ``1`` if ``value > 0`` and ``-1`` if ``value <= 0``.
     */
    template<typename T>
    T sign(const T value) {
        if (value > 0)
            return T(1);
        else
            return T(-1);
    }
}

#endif