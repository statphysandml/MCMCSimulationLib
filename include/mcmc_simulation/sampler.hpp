//
// Created by lukas on 09.01.20.
//

#ifndef MAIN_SAMPLER_HPP
#define MAIN_SAMPLER_HPP

#include <iostream>

#include "util/random.hpp"

struct Sampler
{
    template<typename T>
    T random_state()
    {
//        return self().random_state();
        std::cout << "Function random state needs to be implemented" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    template<typename T>
    T propose_state(T site)
    {
//        return self().propose_state();
        std::cout << "Function propose state needs to be implemented" << std::endl;
        std::exit(EXIT_FAILURE);
    }

/* private:
    Derived& self() {
        return *static_cast<Derived*>(this);
    } */
};

#endif //MAIN_SAMPLER_HPP
