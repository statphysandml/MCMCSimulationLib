#ifndef MEASUREPOLICY_HPP
#define MEASUREPOLICY_HPP

#include <iostream>
#include <complex>
#include <vector>
#include <memory>

namespace mcmc {
    namespace common_measures {

        // Type policies

        template<typename T>
        struct TypePolicy {
        public:
            static double realv(const T state) {
                return state;
            }

            static double imagv(const T state) {
                return state;
            }

            static std::string conf(const T state) {
                return std::to_string(state) + " ";
            }
        };

        template<>
        struct TypePolicy<std::complex<double> > {
        public:
            static double realv(const std::complex<double> state) {
                return state.real();
            }

            static double imagv(const std::complex<double> state) {
                return state.imag();
            }

            static std::string conf(const std::complex<double> state) {
                return std::to_string(state.real()) + " " + std::to_string(state.imag()) + " ";
            }
        };

        template<>
        struct TypePolicy<const std::complex<double> > {
        public:
            static double realv(const std::complex<double> state) {
                return state.real();
            }

            static double imagv(const std::complex<double> state) {
                return state.imag();
            }

            static std::string conf(const std::complex<double> state) {
                return std::to_string(state.real()) + " " + std::to_string(state.imag()) + " ";
            }
        };

        // Measure policies

        // Measure policy base class
        template<typename SB>
        struct MeasurePolicy {
            virtual std::string measure(const SB &system) = 0;

            virtual std::string name() = 0;
        };

        template<typename SB>
        struct MeasureMeanPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                auto sum = decltype(system[0]){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += system[i];
                return std::to_string(TypePolicy<decltype(sum)>::realv(sum) * 1.0 / system.size());
            }

            std::string name() {
                return "Mean";
            }
        };

        template<typename SB>
        struct MeasureMeanImagPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                auto sum = decltype(system[0]){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += system[i];
                return std::to_string(TypePolicy<decltype(sum)>::imagv(sum) * 1.0 / system.size());
            }

            std::string name() {
                return "MeanImag";
            }
        };

        template<typename SB>
        struct MeasureAbsPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                double sum = 0.0;
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
                return std::to_string(abs(TypePolicy<decltype(sum)>::realv(sum)) * 1.0 / system.size());
            }

            std::string name() {
                return "AbsMean";
            }
        };

        template<typename SB>
        struct MeasureStdPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                auto sum = decltype(system[0]){0};
                //sum += 1;
                //std::cout << a << " " << sum << std::endl;
                return std::to_string(TypePolicy<decltype(sum)>::realv(system[0]));
            }

            std::string name() {
                return "Std";
            }
        };

        template<typename SB>
        struct MeasureSecondMomentPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                double sum = decltype(system[0]){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += pow(system[i], 2.0);
                return std::to_string(sum * 1.0 / system.size());
            }

            std::string name() {
                return "SecondMoment";
            }
        };

        template<typename SB>
        struct MeasureFourthMomentPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                double sum = decltype(system[0]){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += pow(system[i], 4.0);
                return std::to_string(sum * 1.0 / system.size());
            }

            std::string name() {
                return "FourthMoment";
            }
        };

        template<typename SB>
        struct MeasureConfigPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                std::string config;
                for (uint i = 0; i < system.size(); i++) {
                    config += TypePolicy<decltype(system[i])>::conf(system[i]);
                }
                return config;
            }

            std::string name() {
                return "Config";
            }
        };

        template<typename SB>
        struct MeasureRealConfigPolicy : public MeasurePolicy<SB> {
            std::string measure(const SB &system) override {
                std::string config = "";
                for (uint i = 0; i < system.size(); i++)
                    config += std::to_string(TypePolicy<decltype(system[i])>::realv(system[i])) + " ";
                return config.substr(0, config.size() - 1);
            }

            std::string name() {
                return "RealConfig";
            }
        };
    }
}

#endif
