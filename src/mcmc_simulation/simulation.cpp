//
// Created by lukas on 31.07.20.
//

#include "../../include/mcmc_simulation/simulation.hpp"

namespace mcmc {
    namespace simulation {

        json::iterator get_running_parameter_iterator(json &params, std::vector<std::string> running_parameter_path) {
            uint i = 0;
            auto it = params.find(running_parameter_path[i]);
            while (i < running_parameter_path.size() - 1) {
                i++;
                auto it_new = it->find(running_parameter_path[i]);
                if (it_new == it->end()) {
                    std::cerr << "Running parameter not found." << std::endl;
                    std::exit(EXIT_FAILURE);
                } else
                    it = it_new;
            }
            return it;
        }

        json update_running_parameter(const json params, std::vector<std::string> running_parameter_path,
                                      double new_running_parameter) {
            std::cout << " -- Updating the running parameter to " << new_running_parameter << " --" << std::endl;
            json new_params = params;
            json::iterator it = get_running_parameter_iterator(new_params, running_parameter_path);
            *it = new_running_parameter;
            return new_params;
        }

    }
}