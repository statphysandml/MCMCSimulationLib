//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXPECTATION_VALUE_HPP
#define MAIN_EXPECTATION_VALUE_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "../../mcmc_simulation/markov_chain.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace execution {

        class ExpectationValueParameters : public param_helper::params::Parameters {
        public:
            explicit ExpectationValueParameters(const json params_) : Parameters(params_) {
                correlation_time_rel_results_path = get_entry<std::string>("correlation_time_rel_results_path", "None");
                equilibriate_rel_results_path = get_entry<std::string>("equilibriate_rel_results_path", "None");
                measure_interval = get_entry<uint>("measure_interval", 100);
                number_of_measurements = get_entry<uint>("number_of_measurements", 1000);
                start_measuring = get_entry<uint>("start_measuring", 0);

                measures = get_entry<json>("measures", {});
                post_measures = get_entry<json>("post_measures", {});
                n_means_bootstrap = get_entry<uint>("n_means_bootstrap", 0);
            }

            ExpectationValueParameters(
                    uint measure_interval_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    json measures_ = {},
                    json post_measures_ = {},
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"measure_interval",       measure_interval_},
                         {"number_of_measurements", number_of_measurements_},
                         {"start_measuring",        start_measuring_},
                         {"measures",               measures_},
                         {"post_measures",          post_measures_},
                         {"n_means_bootstrap",      n_means_bootstrap_}}) {}

            ExpectationValueParameters(
                    std::string correlation_time_rel_results_path_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    json measures_ = {},
                    json post_measures_ = {},
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"correlation_time_rel_results_path", correlation_time_rel_results_path_},
                         {"measure_interval",                  0},
                         {"number_of_measurements",            number_of_measurements_},
                         {"start_measuring",                   start_measuring_},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}

            ExpectationValueParameters(
                    std::string equilibriate_rel_results_path_,
                    std::string correlation_time_rel_results_path_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    json measures_ = {},
                    json post_measures_ = {},
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"equilibriate_rel_results_path",     equilibriate_rel_results_path_},
                         {"correlation_time_rel_results_path", correlation_time_rel_results_path_},
                         {"measure_interval",                  0},
                         {"number_of_measurements",            number_of_measurements_},
                         {"start_measuring",                   start_measuring_},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}

            void write_to_file(const std::string &root_dir) {
                Parameters::write_to_file(root_dir, "expectation_value_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "expectation_value";
            }

            std::unique_ptr<mcmc::simulation::MarkovChainParameters>
            generate_markovchain_params(std::string running_parameter = "None", double rp = 0) {
                uint correlation_time = measure_interval; // defaultgut
                if (correlation_time_rel_results_path != "None") {
                    auto correlation_time_results = param_helper::fs::read_parameter_file(
                            correlation_time_rel_results_path + "/", "correlation_time_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << "Looking for correlation time for rp=" << rp << " in correlation_time.json"
                              << std::endl;
                    correlation_time = param_helper::params::entry_by_key<uint>(
                            correlation_time_results["CorrelationTime"], rp_key);
                    std::cout << "Found correlation time: " << correlation_time << std::endl;
                }

                uint equilibriation_time = start_measuring;
                if (equilibriate_rel_results_path != "None") {
                    auto equilibriation_time_results = param_helper::fs::read_parameter_file(
                            equilibriate_rel_results_path, "equilibriate_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << "Looking for equilibriation time for rp=" << rp << " in equilibriate_results.json"
                              << std::endl;
                    equilibriation_time = param_helper::params::entry_by_key<uint>(
                            equilibriation_time_results["EqulibriationTime"], rp_key);
                }

                return std::make_unique<mcmc::simulation::MarkovChainParameters>(
                        correlation_time,
                        number_of_measurements,
                        1,
                        equilibriation_time,
                        "hot");
            }

            json &get_measures() {
                return measures;
            }

        private:
            std::string equilibriate_rel_results_path;
            std::string correlation_time_rel_results_path;

            uint measure_interval;
            uint number_of_measurements;
            uint start_measuring;
            json measures;
            json post_measures;
            uint n_means_bootstrap;
        };

    }
}

#endif //MAIN_EXPECTATION_VALUE_HPP
