//
// Created by lukas on 03.08.20.
//

#ifndef MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP
#define MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP

#include <boost/filesystem.hpp>

#include "param_helper/params.hpp"

template<typename BaseParameterClass, typename ParameterClass>
json generate_parameter_class_json(BaseParameterClass& base_parameters, std::string param_class_name, std::string rel_config_path) {
    std::string parameter_class_params_path = base_parameters.template get_value_by_key<std::string>(param_class_name + "_path",
                                                                            rel_config_path);
    json parameter_class_params_ = {};

    // Default - No parameter_class_params found in file and no parameter_class_params found in config params
    if (!boost::filesystem::is_regular_file(gcp() + parameter_class_params_path + "/" + param_class_name + ".json") and
        !base_parameters.template haskey(param_class_name))
        parameter_class_params_ = {{param_class_name + "_name", ParameterClass::name()}};  // Generate default parameter_class params
    // From json object - Load class parameters from object -> if a param_class_name + "_params.json" file exists, parameter are always loaded from this file!
    else if (base_parameters.template haskey(param_class_name)) // -> load json file if possible
    // else if (!boost::filesystem::is_regular_file(gcp() + parameter_class_params_path + "/" + param_class_name + "_params.json"))
    {
        parameter_class_params_ = base_parameters.template get_value_by_key<json>(param_class_name);
        parameter_class_params_[param_class_name + "_name"] = ParameterClass::name();
        // base_parameters.template delete_entry(param_class_name); -> This line can result in errors if parameter class is loaded more than once
    }
    // From file - Load class parameters from file -> for usage with execution mode
    else {
        parameter_class_params_ = Parameters::read_parameter_file(parameter_class_params_path, param_class_name);
        // Verify integrity
        if (parameter_class_params_[param_class_name + "_name"] != ParameterClass::name())
        {
            std::cout << "Parameter class name: '" << parameter_class_params_[param_class_name + "_name"]
                      << "' in config file and actual parameter class name: '" << ParameterClass::name()
                      << "' do not coincide" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Generate parameters
    return parameter_class_params_;
}

#endif //MCMCSIMULATIONLIB_MCMC_PARAMETERS_HPP
