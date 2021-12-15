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
                equilibrium_time_rel_results_path = get_entry<std::string>("equilibrium_time_rel_results_path", "None");
                measure_interval = get_entry<uint>("measure_interval", 1);
                number_of_measurements = get_entry<uint>("number_of_measurements", 1000);
                start_measuring = get_entry<uint>("start_measuring", 0);

                measures = get_entry<std::vector<std::string>>("measures");
                if(measures.empty())
                {
                    std::cerr << "Measures cannot be empty" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                post_measures = get_entry<std::vector<std::string>>("post_measures", {});
                n_means_bootstrap = get_entry<uint>("n_means_bootstrap", 0);
            }

            ExpectationValueParameters(
                    uint measure_interval_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
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
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
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
                    uint measure_interval_,
                    uint number_of_measurements_,
                    std::string equilibrium_time_rel_results_path_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"measure_interval",                  measure_interval_},
                         {"number_of_measurements",            number_of_measurements_},
                         {"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"start_measuring",                   0},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}

            ExpectationValueParameters(
                    std::string correlation_time_rel_results_path_,
                    uint number_of_measurements_,
                    std::string equilibrium_time_rel_results_path_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"correlation_time_rel_results_path", correlation_time_rel_results_path_},
                         {"measure_interval",                  0},
                         {"number_of_measurements",            number_of_measurements_},
                         {"start_measuring",                   0},
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

            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir, const std::string sim_root_dir,
                const std::string running_parameter="None", const double rp_minimum=0.0,
                const double rp_maximum=0.0, const int rp_number=0, const json simparams_json={})
            {
                #ifdef RUN_WITH_PYTHON_BACKEND
                py::exec("from mcmctools.modes.expectation_value import expectation_value");
                py::exec("from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func");
                py::exec("from mcmctools.utils.utils import retrieve_rp_keys");
                py::exec(("expectation_value(\
                    measures=" + param_helper::params::merge_list_like<std::string>(measures, post_measures).dump() + ",\
                    running_parameter='" + running_parameter + "',\
                    rp_keys=retrieve_rp_keys(running_parameter='" + running_parameter + "',\
                        rp_minimum=" + std::to_string(rp_minimum) + ",\
                        rp_maximum=" + std::to_string(rp_maximum) + ",\
                        rp_number=" + std::to_string(rp_number) + "),\
                    rel_data_dir='" + rel_data_dir + "',\
                    number_of_measurements=" + std::to_string(number_of_measurements) + ",\
                    n_means_bootstrap=" + std::to_string(n_means_bootstrap) + ",\
                    rel_results_dir='" + rel_results_dir + "',\
                    sim_base_dir='" + param_helper::fs::prfs::project_root() + sim_root_dir + "',\
                    custom_measures_func=get_custom_measures_func(), custom_measures_args='" + simparams_json.dump() + "',\
                    custom_load_data_func=get_custom_load_data_func(), custom_load_data_args='" + simparams_json.dump() + "')").c_str());
                #endif
            }

            std::unique_ptr<mcmc::simulation::MarkovChainParameters>
            generate_markovchain_params(std::string running_parameter = "None", double rp = 0) {
                uint correlation_time = measure_interval; // default
                if (correlation_time_rel_results_path != "None") {
                    auto correlation_time_results = param_helper::fs::read_parameter_file(
                            correlation_time_rel_results_path + "/", "correlation_time_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << " -- Looking for correlation time for rp=" << rp << " in correlation_time_results.json --"
                              << std::endl;
                    correlation_time = param_helper::params::entry_by_key<uint>(
                            correlation_time_results["CorrelationTime"], rp_key);
                }
                
                uint equililbrium_time = start_measuring;
                if (equilibrium_time_rel_results_path != "None") {
                    auto equilibrium_time_results = param_helper::fs::read_parameter_file(
                            equilibrium_time_rel_results_path, "equilibrium_time_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << " -- Looking for equilibrium time for rp=" << rp << " in equilibrium_time_results.json --"
                              << std::endl;
                    equililbrium_time = param_helper::params::entry_by_key<uint>(
                            equilibrium_time_results["EquilibriumTime"], rp_key);
                }

                return std::make_unique<mcmc::simulation::MarkovChainParameters>(
                        correlation_time,
                        number_of_measurements,
                        1,
                        equililbrium_time,
                        "hot");
            }

            std::vector<std::string> get_measures() {
                return measures;
            }

        private:
            std::string equilibrium_time_rel_results_path;
            std::string correlation_time_rel_results_path;

            uint measure_interval;
            uint number_of_measurements;
            uint start_measuring;
            std::vector<std::string> measures;
            std::vector<std::string> post_measures;
            uint n_means_bootstrap;
        };

    }
}

#endif //MAIN_EXPECTATION_VALUE_HPP
