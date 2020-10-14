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

    double get_eps() const
    {
        return eps;
    }

    template<typename T>
    std::pair<double, double> get_integration_bounds(const T& site) const
    {
        return std::pair<double, double> (site.real() - eps, site.real() + eps);
    }

    struct transformer_func
    {
#ifdef THRUST
        __host__ __device__
#endif
        double operator() (const double val)
        {
            return val;
        }
    };

#ifdef THRUST
    __host__ __device__
#endif
    double jacobian(const double x)
    {
        return 1.0;
    }

    const static std::string name() {
        return "UniformSampler";
    }

    const double eps;
    std::uniform_real_distribution<double> uniform;

    transformer_func transformer;
};

#endif //MAIN_UNIFORM_SAMPLER_HPP
