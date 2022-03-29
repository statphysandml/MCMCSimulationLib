#ifndef MAIN_CORRELATION_TIME_HPP
#define MAIN_CORRELATION_TIME_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "mcmc_simulation/markov_chain.hpp"
#include "mcmc_simulation/util/python_integration.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace mode {
        /** @brief Computes the autocorrelation time of the studied system
         *
         * This class defines necessary parameters for running a MCMC simulation
         * for determining the autocorrelation of the model according to Eq. (3.21) from
         * <em>Newman - Monte Carlo Methods in Statistical Physics</em>:
         * \f{eqnarray*}{
         *  \chi(t) &=& \frac{1}{t_{\text{max}} - t}\sum_{t'=0}^{t_{\text{max}} - t} m(t') m(t' + t) \\&&- \frac{1}{t_{\text{max}} - t}\sum_{t'=0}^{t_{\text{max}} - t} m(t') \times \frac{1}{t_{\text{max}} - t}\sum_{t'=0}^{t_{\text{max}} - t} m(t' + t)\,,
         * \f}
         * where \f$t\f$ refers to the computer time, \f$t_{\text{max}}\f$ to the ``maximum_correlation_time``
         * and \f$m\f$ to the ``measure`` used to evaluate the autocorrelation time.
         * 
         * Note that some of the parameters are only needed for the respective evaluation which
         * is executed afterwards in Python.
         */
        class CorrelationTime : public param_helper::params::Parameters {
        public:
            explicit CorrelationTime(const json params_) : Parameters(params_) {
                minimum_sample_size = get_entry<uint>("minimum_sample_size", 100);
                maximum_correlation_time = get_entry<uint>("maximum_correlation_time", 1000);
                equilibrium_time_rel_results_path = get_entry<std::string>("equilibrium_time_rel_results_path", "None");
                start_measuring = get_entry<uint>("start_measuring", 0);
                measure = get_entry<std::string>("measure", "Mean");
                starting_mode = get_entry<std::string>("starting_mode", "hot");
            }

            /** @brief Constructor defining all important parameters for the computation of the correlation time
             *
             * @param minimum_sample_size_ Minimum number of Monte Carlo samples used to compute an average of the considered measure
             * @param maximum_correlation_time_ Maximum measurable correlation time
             * @param start_measuring_ Number of Monte Carlo sweeps before starting with the first measurement
             * @param measure_ Measure used to evalute the autocorrelation time
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             */

            CorrelationTime(
                    uint minimum_sample_size_=100,
                    uint maximum_correlation_time_=1000,
                    uint start_measuring_=0,
                    std::string measure_="Mean",
                    std::string starting_mode_ = "hot"
            ) : CorrelationTime(
                    json{{"minimum_sample_size",      minimum_sample_size_},
                         {"maximum_correlation_time", maximum_correlation_time_},
                         {"start_measuring",          start_measuring_},
                         {"measure",                 measure_},
                         {"starting_mode",          starting_mode_}}) {}

            /** @brief Same as above with the exception that the time to equilibrium before the first measurement,
             * which can be computed by the EquilibriumTime mode, is loaded from file.
             *
             * @param minimum_sample_size_ Minimum number of Monte Carlo samples used to compute an average of the considered measure
             * @param maximum_correlation_time_ Maximum measurable correlation time
             * @param equilibrium_time_rel_results_path_ Relative path (with respect to the top-level directory of the project) to the equilibrium_time_results.json results file
             * @param measure_ Measure used to evalute the autocorrelation time
             * @param starting_mode_ Defines how the Markov chain is initialized; Possible values are "hot" or "cold".
             */
            CorrelationTime(                    
                    uint minimum_sample_size_,
                    uint maximum_correlation_time_,
                    std::string equilibrium_time_rel_results_path_,
                    std::string measure_="Mean",
                    std::string starting_mode_ = "hot"
            ) : CorrelationTime(
                    json{{"minimum_sample_size",      minimum_sample_size_},
                         {"maximum_correlation_time", maximum_correlation_time_},
                         {"equilibrium_time_rel_results_path", equilibrium_time_rel_results_path_},
                         {"start_measuring",          0},
                         {"measure",                 measure_},
                         {"starting_mode",          starting_mode_}}) {}

            /** @brief Write the correlation time parameters as correlation_time_params.json into rel_root_dir
             *
             * @param rel_root_dir Relative path to the project_root_dir for storing configuration files
             * @returns None
             */
            void write_to_file(const std::string &rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "correlation_time_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "correlation_time";
            }

            /** @brief Evaluates the correlation time by calling the respective Python functions and writes the results to file
             *
             * For the evaluation to work, one needs to enable Python in CMake and initialize Python by mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH) in the main function.
             * 
             * @param rel_data_dir Relative path to the project_root_dir (set by param_helper::proj::set_relative_path_to_project_root_dir("../")) for storing the MCMC simulation data
             * @param rel_results_dir Relative path to the project_root_dir for storing the results
             * @param running_parameter Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter
             * @returns None
             */
            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir,
                const std::string running_parameter="None", const std::vector<double>& rp_intervals=std::vector<double>{0.0}, const json simparams_json={})
            {
                #ifdef PYTHON_BACKEND
                py::exec("from mcmctools.modes.correlation_time import correlation_time");
                py::exec("from mcmctools.loading.custom_function_support import get_custom_load_data_func");
                py::exec(("correlation_time(\
                    minimum_sample_size=" + std::to_string(minimum_sample_size) + ",\
                    maximum_correlation_time=" + std::to_string(maximum_correlation_time) + ",\
                    measure='" + measure + "',\
                    running_parameter='" +running_parameter + "',\
                    rp_values=" + json(rp_intervals).dump() + ",\
                    rel_data_dir='" + rel_data_dir + "',\
                    rel_results_dir='" + rel_results_dir + "',\
                    sim_base_dir='" + param_helper::proj::project_root() + "',\
                    fma=fma,\
                    custom_load_data_func=get_custom_load_data_func(), custom_load_data_args='" + simparams_json.dump() + "')").c_str());
                #endif
            }

            mcmc::simulation::MarkovChain
            generate_markov_chain(std::string running_parameter = "None", double rp = 0) {
                uint equilibrium_time = start_measuring;
                if (equilibrium_time_rel_results_path != "None") {
                    auto equilibrium_time_results = param_helper::fs::read_parameter_file(
                            equilibrium_time_rel_results_path, "equilibrium_time_results");
                    std::string rp_key;
                    if (running_parameter == "None")
                        rp_key = "default";
                    else
                        rp_key = std::to_string(rp);
                    std::cout << " -- Looking for equilibrium time for rp=" << rp << " in equilibrium_time_results.json --" /*(in: "
                              << param_helper::proj::get_path_to(equilibrium_time_rel_results_path)
                              << ") --"*/
                              << std::endl;
                    equilibrium_time = param_helper::params::entry_by_key<uint>(
                            equilibrium_time_results["EquilibriumTime"], rp_key);
                }

                return mcmc::simulation::MarkovChain(1, minimum_sample_size + maximum_correlation_time, 1, equilibrium_time, "hot");
            }

            std::vector<std::string> get_measures() {
                return std::vector<std::string>{measure};
            }

        private:
            std::string equilibrium_time_rel_results_path;

            uint minimum_sample_size;
            uint maximum_correlation_time;
            uint start_measuring;
            std::string measure;
            std::string starting_mode;
        };
    }
}

#endif //MAIN_CORRELATION_TIME_HPP
