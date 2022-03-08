#ifndef MAIN_GAUSSIAN_SAMPLER_HPP
#define MAIN_GAUSSIAN_SAMPLER_HPP


#include "../util/random.hpp"


namespace mcmc {
    namespace sampler {

        /** @brief Gaussian distribution sampler with variance 2 * eps
         *
         * Sampler function for sampling, evaluating and integrating a Gaussian distribution.
         */
        struct GaussianSampler {
            GaussianSampler(const double eps_) : eps(eps_) {
                normal = std::normal_distribution<double>(0, 1);
            }

            template<typename T>
            T random_state() {
                return std::sqrt(2 * eps) * normal(mcmc::util::gen);
            }

            template<typename T>
            T cold_state() {
                return T(0);
            }

            template<typename T>
            T propose_state(T site) {
                return site + std::sqrt(2 * eps) * normal(mcmc::util::gen);
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
                return std::pair<double, double>(-1.0, 1.0);
            }

            struct transformer_func {
                double operator()(const double val) {
                    return std::log((1.0 + val) / (1.0 - val));
                }
            };

            double jacobian(const double x) {
                return -2.0 / (std::pow(x, 2.0) - 1.0);
            }

            const static std::string name() {
                return "GaussianSampler";
            }

            const double eps;
            std::normal_distribution<double> normal;

            transformer_func transformer;
        };
    }
}

#endif //MAIN_GAUSSIAN_SAMPLER_HPP
