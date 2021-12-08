//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_CORRELATION_TIME_HPP
#define MAIN_CORRELATION_TIME_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "../../mcmc_simulation/markov_chain.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace execution {

        class CorrelationTimeParameters : public param_helper::params::Parameters {
        public:
            explicit CorrelationTimeParameters(const json params_) : Parameters(params_) {
                minimum_sample_size = get_entry<uint>("minimum_sample_size", 100);
                maximum_correlation_time = get_entry<uint>("maximum_correlation_time", 1000);
                equilibrium_time_rel_results_path = get_entry<std::string>("equilibrium_time_rel_results_path", "None");
                start_measuring = get_entry<uint>("start_measuring", 0);
                measure = get_entry<std::string>("measure", "Mean");
            }

            CorrelationTimeParameters(
                    uint minimum_sample_size_,
                    uint maximum_correlation_time_,
                    uint start_measuring_,
                    std::string measure_="Mean"
            ) : CorrelationTimeParameters(
                    json{{"minimum_sample_size",      minimum_sample_size_},
                         {"maximum_correlation_time", maximum_correlation_time_},
                         {"start_measuring",          start_measuring_},
                         {"measure",                 measure_}}) {}
            
            CorrelationTimeParameters(                    
                    uint minimum_sample_size_,
                    uint maximum_correlation_time_,
                    std::string equilibrium_time_rel_results_path_,
                    std::string measure_="Mean"
            ) : CorrelationTimeParameters(
                    json{{"minimum_sample_size",      minimum_sample_size_},
                         {"maximum_correlation_time", maximum_correlation_time_},
                         {"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"start_measuring",          0},
                         {"measure",                 measure_}}) {}


            void write_to_file(const std::string &root_dir) {
                Parameters::write_to_file(root_dir, "correlation_time_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "correlation_time";
            }

            std::unique_ptr<mcmc::simulation::MarkovChainParameters>
            generate_markovchain_params(std::string running_parameter = "None", double rp = 0) {
                uint equilibrium_time = start_measuring;
                if (equilibrium_time_rel_results_path != "None") {
                    auto equilibrium_time_results = param_helper::fs::read_parameter_file(
                            equilibrium_time_rel_results_path, "equilibrium_time_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << "Looking for equilibrium time for rp=" << rp << " in equilibrium_results.json"
                              << std::endl;
                    equilibrium_time = param_helper::params::entry_by_key<uint>(
                            equilibrium_time_results["EquilibriumTime"], rp_key);
                    std::cout << "Found equilibrium time: " << equilibrium_time << std::endl;
                }

                return std::make_unique<mcmc::simulation::MarkovChainParameters>(
                        1,
                        minimum_sample_size + maximum_correlation_time,
                        1,
                        equilibrium_time,
                        "hot");
            }

            json get_measures() {
                json measures(std::vector<std::string>{measure});
                return measures;
            }

        private:
            std::string equilibrium_time_rel_results_path;

            uint minimum_sample_size;
            uint maximum_correlation_time;
            uint start_measuring;
            std::string measure;
        };
    }
}

#endif //MAIN_CORRELATION_TIME_HPP
