#ifndef RANDOMMT_HPP
#define RANDOMMT_HPP

#include <random>
#include <complex>

static std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems
// initialize Mersennes' twister using rd to generate the seed
static std::mt19937 gen(rd());

template<typename T>
T sign(const T value) {
    if(value > 0)
        return T(1);
    else
        return T(-1.0);
}

#endif