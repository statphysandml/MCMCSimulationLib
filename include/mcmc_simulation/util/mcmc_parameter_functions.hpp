//
// Created by lukas on 03.08.20.
//

#ifndef MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP
#define MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP

#include "param_helper/params.hpp"

namespace mcmc {
    namespace util {

        template<typename BaseParameterClass, typename ParameterClass>
        json generate_parameter_class_json(BaseParameterClass &base_parameters, std::string param_class_name) {

            json parameter_class_params_ = {};

            // From json object - Load class parameters from object -> if a param_class_name + "_params.json" file exists, parameter are always loaded from this file!
            if (base_parameters haskey(param_class_name)) {
                std::cout << "Load params with name " << param_class_name << " from json object" << std::endl;
                parameter_class_params_ = base_parameters.template get_entry<json>(param_class_name);
                parameter_class_params_[param_class_name + "_name"] = ParameterClass::name();
            } else if (base_parameters haskey(param_class_name + "_path") and
                       param_helper::fs::check_if_parameter_file_exists(
                               base_parameters.template get_entry<std::string>(param_class_name + "_path"),
                               param_class_name, true)) // -> load json file if possible
            {
                std::string parameter_class_params_path = base_parameters.template get_entry<std::string>(
                        param_class_name + "_path");
                std::cout << "Load params with name " << param_class_name << " from file from "
                          << parameter_class_params_path << std::endl;
                parameter_class_params_ = param_helper::fs::read_parameter_file(parameter_class_params_path,
                                                                               param_class_name);
                // Verify integrity
                if (parameter_class_params_[param_class_name + "_name"] != ParameterClass::name()) {
                    std::cout << "Parameter class name: '" << parameter_class_params_[param_class_name + "_name"]
                              << "' in config file and actual parameter class name: '" << ParameterClass::name()
                              << "' do not coincide" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            }
                // Default - No parameter_class_params found in file and no parameter_class_params found in config params
            else {
                std::cout << "Generate default parameter_class params" << std::endl;
                parameter_class_params_ = {{param_class_name +
                                            "_name", ParameterClass::name()}};  // Generate default parameter_class params
            }

            return parameter_class_params_;
        }

    }
}

#endif //MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP
