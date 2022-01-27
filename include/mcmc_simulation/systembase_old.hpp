//
// Created by lukas on 04.12.20.
//

#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP

#include <vector>

#include <param_helper/params.hpp>
#include "measure_policy.hpp"

namespace mcmc {
    namespace simulation {

        class SystemBaseParameters : public param_helper::params::Parameters {
        public:
            SystemBaseParameters(const json params_) : Parameters(params_),
                                                       measures(get_entry<std::vector<std::string>>("measures", {}))
            {}

            // At the moment it is not allowed to overwrite the following two functions - not working
            static const std::string name() {
                return "systembase_params";
            }

            // Optional
            virtual void write_to_file(const std::string &root_dir) {
                Parameters::write_to_file(root_dir, name());
            }

            // Function that enables to add parameters of other used classes with parameters
            virtual param_helper::params::Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            // Implement this function with your respective "SystemClass" as "std::unique_ptr<SystemClass> generate() { return std::make_unique<SystemClass>(*this); }"
            template<typename System>
            std::unique_ptr<System> generate() {
                std::cerr << "This function needs to be defined for your associate system class." << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // Enables execution modes to temporarily use their own measures -> only works with systembase_measures
            void update_measures(const std::vector<std::string> measures_)
            {
                measures = measures_;
                params["measures"] = measures_;
            }

            const std::vector<std::string> get_measures() const
            {
                return measures;
            }

        protected:
            std::vector<std::string> measures;
        };

        template<typename Derived>
        class SystemBase {
        public:
            void update(uint measure_interval = 1) {
                systembase().update_step(measure_interval);
            }

            void init(std::string starting_mode) {
                systembase().initialize(starting_mode);
            }

            std::vector<std::string> measure() {
                return systembase().perform_measure();
            }

            std::vector<std::string> measure_names() {
                return systembase().get_measure_names();
            }

            auto size() const {
                return systembase().get_size();
            }

            auto operator[](int i) const {
                return systembase().at(i);
            }

            auto &operator[](int i) {
                return systembase().at(i);
            }

            auto operator()() const {
                return systembase().get_system_representation();
            }

            auto &operator()() {
                return systembase().get_system_representation();
            }

            // Predefined measure method
            virtual std::vector<std::string> perform_measure() {
                std::vector<std::string> results;
                for (auto const &element: measures) {
                    results.push_back(element->measure(systembase()));
                }
                return results;
            }

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

            //    virtual void save_config(int i) = 0;

        protected:
            virtual std::vector< std::unique_ptr<measures::Measure<Derived>> > generate_systembase_measures(const std::vector<std::string>& measure_names);

            void concat_measures(std::vector<std::unique_ptr<measures::Measure<Derived>>> &measures_);

            // Stores all measure objects - these objects need to created by the derived class by calling generate_measures in void initialize()
            std::vector<std::unique_ptr<measures::Measure<Derived>>> measures;

        private:
            Derived &systembase() {
                return *static_cast<Derived *>(this);
            }

            const Derived &systembase() const {
                return *static_cast<const Derived *>(this);
            }
        };

        template<typename Derived>
        std::vector< std::unique_ptr<measures::Measure<Derived>> > SystemBase<Derived>::generate_systembase_measures(const std::vector<std::string>& measure_names)
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
        void SystemBase<Derived>::concat_measures(
                std::vector<std::unique_ptr<measures::Measure<Derived>>> &measures_) {
            std::move(begin(measures_), end(measures_), std::inserter(measures, end(measures)));
        }
    }
}

#endif //MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
