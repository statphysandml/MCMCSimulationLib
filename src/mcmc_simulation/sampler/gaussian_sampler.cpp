#include "../../../include/mcmc_simulation/sampler/gaussian_sampler.hpp"

namespace mcmc {
    namespace sampler {

        template<>
        std::complex<double> GaussianSampler::random_state() {
            return {std::sqrt(2 * eps) * normal(mcmc::util::gen), 0.1};
        }

        template<>
        std::complex<double> GaussianSampler::propose_state(std::complex<double> site) {
            return site + std::complex<double>{std::sqrt(2 * eps) * normal(mcmc::util::gen), 0.0};
        }

    }
}