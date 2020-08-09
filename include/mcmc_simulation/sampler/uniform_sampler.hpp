#ifndef MAIN_UNIFORM_SAMPLER_HPP
#define MAIN_UNIFORM_SAMPLER_HPP

#include "../sampler.hpp"


struct UniformSampler //  : public Sampler
{
    UniformSampler(const double eps_) : eps(eps_)
    {
        uniform = std::uniform_real_distribution<double>(-1.0, 1.0);
    }

    template<typename T>
    T random_state()
    {
        return eps * uniform(gen);
    }

    template<typename T>
    T propose_state(T site)
    {
        return site + eps * uniform(gen);
    }

    const static std::string name() {
        return "UniformSampler";
    }

    double get_eps() const
    {
        return eps;
    }
    const double eps;
    std::uniform_real_distribution<double> uniform;
};

#endif //MAIN_UNIFORM_SAMPLER_HPP
