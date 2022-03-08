#ifndef MCMCSIMULATIONLIB_DUMMY_SAMPLER_HPP
#define MCMCSIMULATIONLIB_DUMMY_SAMPLER_HPP


#include "../util/random.hpp"


namespace mcmc {
    namespace sampler {
        
        /** @brief Dummy sampler - can be used if a sampler function is needed for a template function, but the sampling is performed in a different way (without a sampler)
         */
        struct DummySampler
        {
            DummySampler(const double eps_) {}

            template<typename T>
            T random_state() {
                return T(0);
            }

            template<typename T>
            T cold_state() {
                return T(0);
            }

            template<typename T>
            T propose_state(T site) {
                return T(0);
            }

            double get_eps() const {
                return 0.0;
            }

            template<typename T>
            T evaluate(T site) const {
                return T(0);
            }

            const static std::string name() {
                return "DummySampler";
            }
        };
    }
}

#endif //MCMCSIMULATIONLIB_DUMMY_SAMPLER_HPP
