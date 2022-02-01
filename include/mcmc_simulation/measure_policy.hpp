#ifndef Measure_HPP
#define Measure_HPP

#include <iostream>
#include <complex>
#include <vector>
#include <memory>


namespace mcmc {
    namespace measures {
        // Measure policies

        /** @brief Base policy class for defining measures.
         * 
         * In addition to the two virtual classes, most of the measures provide a static function called "compute_measure", which returns the
         * measure as a number instead of a string.
        */
        template<typename SB>
        struct Measure {
            /** @brief Returns the measure; Note that for complex types, the mcmc_simulation/util/complex_type.hpp header overloads std::to_string
             * @param system_ Instance of a systembase class
             */
            virtual std::string measure(const SB &system) = 0;

            /** @brief Returns the name, used as an identifier, of the measure. */
            virtual std::string name() = 0;
        };

        /** @brief Returns the mean. */
        template<typename SB>
        struct Mean : public Measure<SB> {
            static auto compute_measure(const SB &system)
            {
                auto sum = decltype(system[0]){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += system[i];
                return sum * (1.0 / double(system.size()));
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() override{
                return "Mean";
            }
        };

        /** @brief Returns the mean of the absolute value of each variable. */
        template<typename SB>
        struct Abs : public Measure<SB> {
            static auto compute_measure(const SB &system)
            {
                auto sum = decltype(std::fabs(system[0])){0};
                for (uint i = 0; i < system.size(); i++)
                    sum += std::fabs(system[i]);
                return sum * (1.0 / double(system.size()));
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() override {
                return "Abs";
            }
        };

        /** @brief Returns the absolute value of the mean. */
        template<typename SB>
        struct AbsMean : public Measure<SB> {
            static auto compute_measure(const SB &system) {
                return std::fabs(Mean<SB>::compute_measure(system));
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() override {
                return "AbsMean";
            }
        };

        /** @brief Returns the mean of the second moment. */
        template<typename SB>
        struct SecondMoment : public Measure<SB> {
            static auto compute_measure(const SB &system)
            {
                auto sum = decltype(system[0] * system[0]){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += system[i] * system[i];
                return sum * (1.0 / double(system.size()));
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() override {
                return "SecondMoment";
            }
        };

        /** @brief Returns the mean of the fourth moment. */
        template<typename SB>
        struct FourthMoment : public Measure<SB> {
            static auto compute_measure(const SB &system)
            {
                auto sum = decltype((system[0] * system[0]) * (system[0] * system[0])){0};
                for(uint i = 0; i < system.size(); i++)
                    sum += (system[i] * system[i]) * (system[i] * system[i]);
                return sum * (1.0 / double(system.size()));
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }

            std::string name() override {
                return "FourthMoment";
            }
        };

        /** @brief Returns the variance. */
        template<typename SB>
        struct Variance : public Measure<SB> {
            static auto compute_measure(const SB &system)
            {
                auto mean = Mean<SB>::compute_measure(system);
                auto second_moment = SecondMoment<SB>::compute_measure(system);
                return second_moment -  mean * mean;
            }

            std::string measure(const SB &system) override {
                return std::to_string(compute_measure(system));
            }
            std::string name() override {
                return "Variance";
            }
        };

        /** @brief Returns all variables of the system in a comma separated list. */
        template<typename SB>
        struct Config : public Measure<SB> {
            static auto compute_measure(const SB &system) {
                std::string config = std::to_string(system[0]);
                for (uint i = 1; i < system.size(); i++) {
                    config += ", " + std::to_string(system[i]);
                }
                return config;
            }

            std::string measure(const SB &system) override {
                return compute_measure(system);
            }

            std::string name() override {
                return "Config";
            }
        };
    }
}

#endif
