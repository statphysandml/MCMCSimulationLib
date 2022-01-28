#ifndef SYSTEMBASE_PARAMETERS_HPP
#define SYSTEMBASE_PARAMETERS_HPP

#include <vector>

#include <param_helper/params.hpp>

namespace mcmc {
    namespace simulation {

        class SystemBaseParameters : public param_helper::params::Parameters {
        public:
            SystemBaseParameters(const json params_) : Parameters(params_),
                                                       measures(get_entry<std::vector<std::string>>("measures", {}))
            {}

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
                std::cerr << "This function needs to be defined for your associate system class." << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // Enables execution modes to temporarily use their own measures -> only works with systembase - not with plain_systembase
            void set_measures(const std::vector<std::string> measures_)
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
    }
}

#endif // SYSTEMBASE_PARAMETERS_HPP