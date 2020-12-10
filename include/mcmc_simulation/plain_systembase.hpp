#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>

#include "param_helper/params.hpp"

namespace mcmc {
    namespace simulation {

        class PlainSystemBaseParameters : public param_helper::params::Parameters {
        public:
            using Parameters::Parameters;

            // Default parameter name for system parameters
            static const std::string name() {
                return "systembase_params";
            }

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
                std::cout << "This function needs to be defined for your associate system class." << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // Enables execution modes to temporarily use their own measures -> only works with systembase - not with plain_systembase
            void update_measures(const json& measures_)
            {
                std::cout << "Usage of measures in execution mode so far only valid for systembase base class" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        };

        template<typename Derived>
        class PlainSystemBase {
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

            const auto size() const {
                return systembase().get_size();
            }

            const auto operator[](int i) const {
                return systembase().at(i);
            }

            auto &operator[](int i) {
                return systembase().at(i);
            }

            const auto operator()() const {
                return systembase().get_system_representation();
            }

            auto &operator()() {
                return systembase().get_system_representation();
            }

            //    virtual void save_config(int i) = 0;

        private:
            Derived &systembase() {
                return *static_cast<Derived *>(this);
            }

            const Derived &systembase() const {
                return *static_cast<const Derived *>(this);
            }
        };
    }
}

#endif