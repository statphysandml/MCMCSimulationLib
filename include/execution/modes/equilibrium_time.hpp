//
// Created by lukas on 14.09.20.
//

#ifndef MCMCSIMULATIONLIB_EQUILIBRIUM_HPP
#define MCMCSIMULATIONLIB_EQUILIBRIUM_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "../../mcmc_simulation/markov_chain.hpp"


using json = nlohmann::json;

namespace mcmc {
    namespace execution {

        class EquilibriumTimeParameters : public param_helper::params::Parameters {
        public:
            explicit EquilibriumTimeParameters(const json params_) : Parameters(params_) {
                sample_size = get_entry<uint>("sample_size", 100);
                number_of_steps = get_entry<uint>("number_of_steps", 1000);
                confidence_range = get_entry<double>("confidence_range", 0.1);
                confidence_window = get_entry<double>("confidence_window", 10);
                measure = get_entry<std::string>("measure", "Mean");
            }

            EquilibriumTimeParameters(
                    uint sample_size_,
                    uint number_of_steps_,
                    double confidence_range_,
                    double confidence_window_,
                    std::string measure_="Mean"
            ) : EquilibriumTimeParameters(
                    json{{"sample_size",    sample_size_},
                         {"number_of_steps", number_of_steps_},
                         {"confidence_range", confidence_range_},
                         {"confidence_window", confidence_window_},
                         {"measure",        measure_}}) {}

            void write_to_file(const std::string &root_dir) {
                Parameters::write_to_file(root_dir, "equilibrium_time_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "equilibrium_time";
            }

            std::unique_ptr<mcmc::simulation::MarkovChainParameters>
            generate_markovchain_params(std::string running_parameter = "None", double rp = 0) {
                return std::make_unique<mcmc::simulation::MarkovChainParameters>(
                        1,
                        number_of_steps,
                        2 * sample_size,
                        0,
                        "alternating");
            }

            json get_measures() {
                json measures(std::vector<std::string>{measure});
                return measures;
            }

        private:
            uint sample_size;
            uint number_of_steps;
            double confidence_range;
            double confidence_window;
            std::string measure;
        };

    }
}

#endif //MCMCSIMULATIONLIB_EQUILIBRIUM_HPP
