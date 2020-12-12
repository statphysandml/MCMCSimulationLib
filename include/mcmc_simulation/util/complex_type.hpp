//
// Created by lukas on 12.12.20.
//

#ifndef MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP
#define MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP

#include "complex"

namespace std
{
    template<typename T>
    std::string to_string(std::complex<T> x)
    {
        return std::to_string(x.real()) + " i" + std::to_string(x.imag());
    }

    template<typename T>
    std::complex<T> operator/(std::complex<T> x, T y)
    {
        return {x.real() / y, x.imag() / y};
    }

    /* template<typename T>
    double fabs(std::complex<T> x)
    {
        return std::fabs(x);
    } */
}

#endif //MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP
