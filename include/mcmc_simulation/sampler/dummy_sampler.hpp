//
// Created by lukas on 05.10.20.
//

#ifndef MCMCSIMULATIONLIB_DUMMY_SAMPLER_HPP
#define MCMCSIMULATIONLIB_DUMMY_SAMPLER_HPP


#include "../sampler.hpp"

namespace mcmc {
    namespace sampler {

        struct DummySampler //  : Sampler
        {
            DummySampler(const double eps_) {}

            template<typename T>
            T random_state() {
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
