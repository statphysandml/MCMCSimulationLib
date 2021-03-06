//
// Created by lukas on 21.02.20.
//

#ifndef MAIN_HAT_FUNCTION_SAMPLER_HPP
#define MAIN_HAT_FUNCTION_SAMPLER_HPP

#include "../sampler.hpp"


// ToDo: Problem -> child functions are not called with current implemented pattern...
/* options:
- Go back to crtp pattern -> look into webpage where this is explained
- Remove inheritance and introduce static_asserts -> does this work properly if function is not used actively?
- Use enable_if or something similar... */

namespace mcmc {
    namespace sampler {

        struct HatFunctionSampler //  : Sampler
        {
            HatFunctionSampler(const double eps_) : eps(eps_) {
                uniform = std::uniform_real_distribution<double>(0, 1);
            }

            template<typename T>
            T random_state() {
                auto r = uniform(mcmc::util::gen);
                if (r < 0.5)
                    return -1.0 * eps * (1.0 - sqrt(2 * r));
                else
                    return 1.0 * eps * (1.0 - sqrt(2.0 * (1.0 - r)));
            }

            template<typename T>
            T propose_state(T site) {
                return site + random_state<T>();
            }

            double get_eps() const {
                return eps;
            }

            template<typename T>
            T evaluate(T site) const {
                return 1.0 / sqrt(2 * M_PI) * std::exp(-1.0 * std::pow(site, 2) / 2.0);
            }

            template<typename T>
            std::pair<double, double> get_integration_bounds(const T &site) const {
                return std::pair<double, double>(site.real() - eps, site.real() + eps);
            }

            struct transformer_func {
/* #ifdef THRUST -> needs to be fixed
        __host__ __device__
#endif */
                double operator()(const double val) {
                    return val;
                }
            };

/* #ifdef THRUST -> needs to be fixed
        __host__ __device__
#endif */
            double jacobian(const double x) {
                return 1.0;
            }

            const static std::string name() {
                return "HatFunctionSampler";
            }

            const double eps;
            std::uniform_real_distribution<double> uniform;

            transformer_func transformer;
        };
    }
}

#endif //MAIN_HAT_FUNCTION_SAMPLER_HPP
