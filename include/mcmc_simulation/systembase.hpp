#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>
#include <variant>


#include <param_helper/params.hpp>

namespace mcmc {
    namespace simulation {
        template<typename Derived>
        class SystemBase : public param_helper::params::Parameters {
        public:
            SystemBase(const json params_) : Parameters(params_),
                                             measures(get_entry<std::vector<std::string>>("measures", {}))
            {}

            // Parameter related functions

            static const std::string name() {
                return "systembase_params";
            }

            virtual void write_to_file(const std::string &rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, name());
            }

            // Function that enables to add parameters of other used classes with parameters
            virtual param_helper::params::Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            /* // Implement this function with your respective "SystemClass" as "std::unique_ptr<SystemClass> generate() { return std::make_unique<SystemClass>(*this); }"
            template<typename System>
            std::unique_ptr<System> generate() {
                std::cerr << "This function needs to be defined for your associate system class." << std::endl;
                std::exit(EXIT_FAILURE);
            } */

            // Enables execution modes to temporarily use their own measures -> only works with systembase - not with plain_systembase
            void set_measures(const std::vector<std::string> measures_)
            {
                measures = measures_;
                params["measures"] = measures_;
            }

            const std::vector<std::string> measure_names() const {
                return measures;
            }

            // System related functions (CRTP pattern)

            void init(std::string starting_mode) {
                systembase().initialize(starting_mode);
            }

            void init_measurements(std::string starting_mode, uint rep=1) {
                systembase().initialize_measurements(starting_mode, rep);
            }
            
            void update(uint measure_interval = 1) {
                systembase().update_step(measure_interval);
            }

            auto measure() {
                return systembase().perform_measurements();
            }

            void fina_measurements(std::string starting_mode, uint rep=1) {
                systembase().finalize_measurements(starting_mode, rep);
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
            
            // Helper functions for pybind11
            template<typename T>
            void set_system_elem(int i, T &val)
            {
                this->operator[](i) = val;
            }

            template<typename T>
            void set_system(T &val)
            {
                this->operator()() = val;
            }

            std::vector<std::string> measures;

        protected:
            Derived &systembase() {
                return *static_cast<Derived *>(this);
            }

            const Derived &systembase() const {
                return *static_cast<const Derived *>(this);
            }
        };
    }
}

#endif // SYSTEMBASE_HPP