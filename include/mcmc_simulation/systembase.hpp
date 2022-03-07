#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>
#include <variant>


#include <param_helper/params.hpp>

namespace mcmc {
    namespace simulation {
        
        template<typename Derived>
        class SystemBase {
        public:
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

            std::vector<std::string> measure_names() {
                return systembase().get_measure_names();
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