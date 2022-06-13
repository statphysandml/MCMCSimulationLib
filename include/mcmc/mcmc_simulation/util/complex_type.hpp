#ifndef MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP
#define MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP

#include <complex>

namespace std
{
    template<typename T>
    std::string to_string(std::complex<T> x)
    {
        return std::to_string(x.real()) + " i" + std::to_string(x.imag());
    }
}

#endif //MCMCSIMULATIONLIB_COMPLEX_TYPE_HPP
