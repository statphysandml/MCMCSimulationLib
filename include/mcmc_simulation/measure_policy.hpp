#ifndef MEASUREPOLICY_HPP
#define MEASUREPOLICY_HPP

#include <iostream>
#include <complex>
#include <vector>
#include <memory>

namespace mcmc {
    namespace common_measures {
        // Measure policies

        // Measure policy base class
        template<typename SB>
        struct MeasurePolicy {
            virtual std::string measure(const SB &system) = 0;

            virtual std::string name() = 0;
        };

        template<typename SB>
        struct MeasureMeanPolicy : public MeasurePolicy<SB> {
            auto compute_measure(const SB &system)
            {
                auto sum = decltype(system[0]){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += system[i];
                return sum * 1.0 / system.size();
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() {
                return "Mean";
            }
        };

        template<typename SB>
        struct MeasureAbsPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                auto sum = decltype(std::fabs(system[0])){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += std::fabs(system[i]);
                return std::to_string(sum * 1.0 / system.size());
            }

            std::string name() {
                return "Abs";
            }
        };

        template<typename SB>
        struct MeasureAbsMeanPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                auto sum = decltype(system[0]){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += system[i];
                return std::to_string(abs(sum * 1.0 / system.size()));
            }

            std::string name() {
                return "AbsMean";
            }
        };

        template<typename SB>
        struct MeasureSecondMomentPolicy : public MeasurePolicy<SB> {
            auto compute_measure(const SB &system)
            {
                auto sum = decltype(system[0] * system[0]){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += system[i] * system[i];
                return sum * 1.0 / system.size();
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() {
                return "SecondMoment";
            }
        };

        template<typename SB>
        struct MeasureFourthMomentPolicy : public MeasurePolicy<SB> {
            auto compute_measure(const SB &system)
            {
                auto sum = decltype((system[0] * system[0]) * (system[0] * system[0])){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += (system[i] * system[i]) * (system[i] * system[i]);
                return sum * 1.0 / system.size();
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() {
                return "FourthMoment";
            }
        };

        template<typename SB>
        struct MeasureVariancePolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                MeasureMeanPolicy<SB> measure_mean;
                auto mean = measure_mean.compute_measure(system);
                MeasureSecondMomentPolicy<SB> measure_second_moment;
                auto second_moment = measure_second_moment.compute_measure(system);
                return std::to_string(second_moment -  mean * mean);
            }

            std::string name() {
                return "Variance";
            }
        };

        template<typename SB>
        struct MeasureConfigPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                std::string config = std::to_string(system[0]);
                for (uint i = 1; i < system.size(); i++) {
                    config += ", " + std::to_string(system[i]);
                }
                return config;
            }

            std::string name() {
                return "Config";
            }
        };
    }
}

#endif
