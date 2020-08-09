//
// Created by lukas on 09.01.20.
//

#ifndef MAIN_GAUSSIAN_SAMPLER_HPP
#define MAIN_GAUSSIAN_SAMPLER_HPP

#include "../sampler.hpp"


// ToDo: Problem -> child functions are not called with current implemented pattern...
/* options:
- Go back to crtp pattern -> look into webpage where this is explained
- Remove inheritance and introduce static_asserts -> does this work properly if function is not used actively?
- Use enable_if or something similar... */

struct GaussianSampler //  : Sampler
{
    GaussianSampler(const double eps_) : eps(eps_)
    {
        normal = std::normal_distribution<double> (0,1);
    }

    template<typename T>
    T random_state()
    {
        return std::sqrt(2 * eps) * normal(gen);
    }

    template<typename T>
    T propose_state(T site)
    {
        return site + std::sqrt(2 * eps) * normal(gen);
    }

    double get_eps() const
    {
        return eps;
    }

    template<typename T>
    T evaluate(T site) const
    {
        return 1.0/sqrt(2 * M_PI) * std::exp(-1.0 * std::pow(site, 2)/2.0);
    }

    const static std::string name() {
        return "GaussianSampler";
    }

    const double eps;
    std::normal_distribution<double> normal;
};

#endif //MAIN_GAUSSIAN_SAMPLER_HPP
