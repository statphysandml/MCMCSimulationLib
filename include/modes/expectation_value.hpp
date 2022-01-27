//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXPECTATION_VALUE_HPP
#define MAIN_EXPECTATION_VALUE_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "mcmc_simulation/markov_chain.hpp"
#include "mcmc_simulation/util/python_integration.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace mode {
            
        /** @brief Prepares the computation of expectation values
         *
         * This class defines necessary parameters for running a MCMC simulation
         * for computing expectation values. Note that some of the parameters are
         * only needed for the respective evaluation which is executed afterwards
         * in Python.
         */
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
                starting_mode = get_entry<std::string>("starting_mode", "hot");
                error_type = get_entry<std::string>("error_type", "statistical");
                n_means_bootstrap = get_entry<uint>("n_means_bootstrap", 0);
                
            }

            /** @brief Standard constructor with custom values for all important parameters
             *
             * @param measure_interval_ Number of Monte Carlo sweeps between measurements (autocorrelation time)
             * @param number_of_measurements_ Total number of measurements
             * @param start_measuring_ Number of Monte Carlo sweeps before starting with the first measurement
             * @param measures_ Measures to be made during the simulation
             * @param post_measures Measures which should be computed afterwards in Python.
             *  Note that for this work the configurations have to be stored as well which can be achieved
             *  by adding "Config" to measures
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             * @param error_type_ Method used to estimate the standard error of the expectation value
             * @param n_means_bootstrap_ Number of bootstrap samples to compute the mean and the error of the measurements, if
             * it is set to zero, the error is computed according to the standard error
             */
            ExpectationValueParameters(
                    uint measure_interval_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    std::string starting_mode_ = "hot",
                    std::string error_type_ = "statistical",
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"measure_interval",       measure_interval_},
                         {"number_of_measurements", number_of_measurements_},
                         {"start_measuring",        start_measuring_},
                         {"measures",               measures_},
                         {"post_measures",          post_measures_},
                         {"starting_mode",          starting_mode_},
                         {"error_type",             error_type_},
                         {"n_means_bootstrap",      n_means_bootstrap_}}) {}

            /** @brief Same as above with the exceptation that the autocorrelation time which was precomputed by the CorrelationTimeParameters mode is loaded from file.
             *
             * @param correlation_time_rel_results_path_ Relative path (with respect to the top-level directory of the project) to the correlation_time_results.json file
             * @param number_of_measurements_ Total number of measurements
             * @param start_measuring_ Number of Monte Carlo sweeps before starting with the first measurement
             * @param measures_ Measures to be made during the simulation
             * @param post_measures Measures which should be computed afterwards in Python.
             *  Note that for this work the configurations have to be stored as well which can be achieved
             *  by adding "Config" to measures
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             * @param error_type_ Method used to estimate the standard error of the expectation value
             * @param n_means_bootstrap_ Number of bootstrap samples to compute the mean and the error of the measurements, if
             * it is set to zero, the error is computed according to the standard error
             */
            ExpectationValueParameters(
                    std::string correlation_time_rel_results_path_,
                    uint number_of_measurements_,
                    uint start_measuring_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    std::string starting_mode_ = "hot",
                    std::string error_type_ = "statistical",
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"correlation_time_rel_results_path", correlation_time_rel_results_path_},
                         {"measure_interval",                  0},
                         {"number_of_measurements",            number_of_measurements_},
                         {"start_measuring",                   start_measuring_},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"starting_mode",                     starting_mode_},
                         {"error_type",                        error_type_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}

            /** @brief Same as above with the exceptation that the number of Monte Carlo sweeps before the measurement which was precomputed by the EquilibriumTimeParameters mode is loaded from file.
             *
             * @param measure_interval_ Number of Monte Carlo sweeps between measurements (autocorrelation time)
             * @param number_of_measurements_ Total number of measurements
             * @param equilibrium_time_rel_results_path_ Relative path (with respect to the top-level directory of the project) to the equilibrium_time_results.json results file
             * @param measures_ Measures to be made during the simulation
             * @param post_measures Measures which should be computed afterwards in Python.
             *  Note that for this work the configurations have to be stored as well which can be achieved
             *  by adding "Config" to measures
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             * @param error_type_ Method used to estimate the standard error of the expectation value
             * @param n_means_bootstrap_ Number of bootstrap samples to compute the mean and the error of the measurements, if
             * it is set to zero, the error is computed according to the standard error
             */
            ExpectationValueParameters(
                    uint measure_interval_,
                    uint number_of_measurements_,
                    std::string equilibrium_time_rel_results_path_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    std::string starting_mode_ = "hot",
                    std::string error_type_ = "statistical",
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"measure_interval",                  measure_interval_},
                         {"number_of_measurements",            number_of_measurements_},
                         {"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"start_measuring",                   0},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"starting_mode",                     starting_mode_},
                         {"error_type",                        error_type_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}
            
            /** @brief Same as above with the exceptation that the number of sweeps before the first measurement as well as the autocorrelation time are loaded from file.
             * Both need to be computed beforhand with the EquilibriumTimeParameters and CorrelationTimeParameters mode.
             *
             * @param correlation_time_rel_results_path_ Relative path (with respect to the top-level directory of the project) to the correlation_time_results.json results file
             * @param number_of_measurements_ Total number of measurements
             * @param equilibrium_time_rel_results_path_ Relative path (with respect to the top-level directory of the project) to the equilibrium_time_results.json results file
             * @param measures_ Measures to be made during the simulation
             * @param post_measures Measures which should be computed afterwards in Python.
             *  Note that for this work the configurations have to be stored as well which can be achieved
             *  by adding "Config" to measures
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             * @param error_type_ Method used to estimate the standard error of the expectation value
             * @param n_means_bootstrap_ Number of bootstrap samples to compute the mean and the error of the measurements, if
             * it is set to zero, the error is computed according to the standard error
             */
            ExpectationValueParameters(
                    std::string correlation_time_rel_results_path_,
                    uint number_of_measurements_,
                    std::string equilibrium_time_rel_results_path_,
                    std::vector<std::string> measures_,
                    std::vector<std::string> post_measures_ = {},
                    std::string starting_mode_ = "hot",
                    std::string error_type_ = "statistical",
                    uint n_means_bootstrap_ = 0
            ) : ExpectationValueParameters(
                    json{{"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"correlation_time_rel_results_path", correlation_time_rel_results_path_},
                         {"measure_interval",                  0},
                         {"number_of_measurements",            number_of_measurements_},
                         {"start_measuring",                   0},
                         {"measures",                          measures_},
                         {"post_measures",                     post_measures_},
                         {"starting_mode",                     starting_mode_},
                         {"error_type",                        error_type_},
                         {"n_means_bootstrap",                 n_means_bootstrap_}}) {}

            /** @brief Write the expectation values as expectation_value_params.json into root_dir
             *
             * @param root_dir Absolute path to the output directory
             * @returns None
             */
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

            /** @brief Evaluate expectation values by calling the respective Python functions and writes the results to file
             *
             * @param x ...Todo
             * @returns None
             */
            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir, const std::string sim_root_dir,
                const std::string running_parameter="None", const std::vector<double>& rp_intervals=std::vector<double>{0.0}, const json simparams_json={})
            {
                #ifdef PYTHON_BACKEND
                py::exec("from mcmctools.modes.expectation_value import expectation_value");
                if(!post_measures.empty())
                    py::exec("from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func");
                else
                    py::exec("from mcmctools.loading.custom_function_support import get_custom_load_data_func; get_custom_measures_func = lambda: None;");
                py::exec(("expectation_value(\
                    measures=" + param_helper::params::merge_list_like<std::string>(measures, post_measures).dump() + ",\
                    running_parameter='" + running_parameter + "',\
                    rp_keys=" + json(rp_intervals).dump() + ",\
                    rel_data_dir='" + rel_data_dir + "',\
                    number_of_measurements=" + std::to_string(number_of_measurements) + ",\
                    error_type='" + error_type + "',\
                    n_means_bootstrap=" + std::to_string(n_means_bootstrap) + ",\
                    rel_results_dir='" + rel_results_dir + "',\
                    sim_base_dir='" + param_helper::proj::project_root() + sim_root_dir + "',\
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
            std::string starting_mode;
            std::string error_type;
            uint n_means_bootstrap;
        };

    }
}

#endif //MAIN_EXPECTATION_VALUE_HPP
