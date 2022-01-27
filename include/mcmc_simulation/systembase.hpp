#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <vector>

#include <param_helper/params.hpp>

namespace mcmc {
    namespace simulation {
        
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

            virtual std::vector<std::string> perform_measure() = 0;

            virtual std::vector<std::string> get_measure_names() = 0;

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