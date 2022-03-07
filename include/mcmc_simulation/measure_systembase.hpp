#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP

#include <vector>

#include <param_helper/params.hpp>
#include "systembase.hpp"
#include "measure_policy.hpp"

namespace mcmc {
    namespace simulation {

        template<typename Derived>
        class MeasureSystemBase : public SystemBase<Derived> {
        public:
            void initialize_measurements(std::string starting_mode, uint rep=1)
            {}

            // Predefined measure method
            virtual std::vector<std::variant<int, double, std::string>> perform_measurements() {
                std::vector<std::variant<int, double, std::string>> results;
                for (auto const &element: measures) {
                    results.push_back(element->measure(this->systembase()));
                }
                return results;
            }

            void finalize_measurements(std::string starting_mode, uint rep=1)
            {}

            virtual std::vector<std::string> get_measure_names() {
                std::vector<std::string> measure_names;
                for (auto const &element: measures) {
                    measure_names.push_back(element->name());
                }
                return measure_names;
            }

            /* By overiding this function one can also add custom generated measures */
            virtual void generate_measures(const std::vector<std::string>& measure_names)
            {
                auto common_defined_measures = generate_systembase_measures(measure_names);
                this->concat_measures(common_defined_measures);
            }

        protected:
            virtual std::vector< std::unique_ptr<measures::Measure<Derived>> > generate_systembase_measures(const std::vector<std::string>& measure_names);

            void concat_measures(std::vector<std::unique_ptr<measures::Measure<Derived>>> &measures_);

            // Stores all measure objects - these objects need to created by the derived class by calling generate_measures in void initialize()
            std::vector<std::unique_ptr<measures::Measure<Derived>>> measures;
        };

        template<typename Derived>
        std::vector< std::unique_ptr<measures::Measure<Derived>> > MeasureSystemBase<Derived>::generate_systembase_measures(const std::vector<std::string>& measure_names)
        {
            std::vector<std::unique_ptr<measures::Measure<Derived>>> measures_{};
            for (auto &measure_name :  measure_names) {
                if (measure_name == "Mean")
                    measures_.push_back(std::make_unique<measures::Mean<Derived>>());
                else if (measure_name == "AbsMean")
                    measures_.push_back(std::make_unique<measures::AbsMean<Derived>>());
                else if (measure_name == "Abs")
                    measures_.push_back(std::make_unique<measures::Abs<Derived>>());
                else if (measure_name == "Variance")
                    measures_.push_back(std::make_unique<measures::Variance<Derived>>());
                else if (measure_name == "SecondMoment")
                    measures_.push_back(std::make_unique<measures::SecondMoment<Derived>>());
                else if (measure_name == "FourthMoment")
                    measures_.push_back(std::make_unique<measures::FourthMoment<Derived>>());
                else if (measure_name == "Config")
                    measures_.push_back(std::make_unique<measures::Config<Derived>>());
            }
            return measures_;
        }

        template<typename Derived>
        void  MeasureSystemBase<Derived>::concat_measures(
                std::vector<std::unique_ptr<measures::Measure<Derived>>> &measures_) {
            std::move(begin(measures_), end(measures_), std::inserter(measures, end(measures)));
        }
    }
}

#endif //MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
