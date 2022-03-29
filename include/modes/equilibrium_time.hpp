#ifndef MCMCSIMULATIONLIB_EQUILIBRIUM_HPP
#define MCMCSIMULATIONLIB_EQUILIBRIUM_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "mcmc_simulation/markov_chain.hpp"
#include "mcmc_simulation/util/python_integration.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace mode {
        /** @brief Prepares the computation of the necessary number of Monte Carlo sweeps for the system to be in equilibrium
         *
         * This class defines necessary parameters for running a MCMC simulation
         * for determining the number of Monte Carlo sweeps the system needs to be in equilibrium.
         * Note that some of the parameters are only needed for the respective evaluation which
         * is executed afterwards in Python.
         * 
         * The time to equilibrium is evaluated by performing several simulations from a hot and a cold start. As soon as the
         * distance between the average evolutions of the hot and the cold ensemble are within a certain confidence range,
         * the configurations are expected to be in equilibrium. The additional parameter ``confidence_window`` allows to
         * perform a smoothening of the averaged evolution based on a local convolution. The reduces the maximum number
         * of Monte Carlo sweeps to be in equillibrium to ``max(confidence_window, number_of_steps) - min(confidence_window, , number_of_steps) + 1``
         */
        class EquilibriumTime : public param_helper::params::Parameters {
        public:
            explicit EquilibriumTime(const json params_) : Parameters(params_) {
                sample_size = get_entry<uint>("sample_size", 100);
                number_of_steps = get_entry<uint>("number_of_steps", 1000);
                confidence_range = get_entry<double>("confidence_range", 0.1);
                confidence_window = get_entry<uint>("confidence_window", 10);
                measure = get_entry<std::string>("measure", "Mean");
            }

            /** @brief Constructor defining all important parameters for a computation of the time to equilibrium
             *
             * @param sample_size_ Number of independent Monte Carlo simulations used to compute the ensemble average evolution from a cold and a hot initial configuration
             * @param number_of_steps_ Number of performed Monte Carlo sweeps
             * @param confidence_range_ Confidence range between the hot and the cold ensemble average
             * @param confidence_window_ Size of the window in computer time (MCMC sweeps) used to smoothing the measured observable over time
             * @param measure_ Measure used to evalute the time to equilibrium
             */
            EquilibriumTime(
                    uint sample_size_ = 100,
                    uint number_of_steps_ = 1000,
                    double confidence_range_ = 0.1,
                    uint confidence_window_ = 10,
                    std::string measure_ = "Mean"
            ) : EquilibriumTime(
                    json{{"sample_size",    sample_size_},
                         {"number_of_steps", number_of_steps_},
                         {"confidence_range", confidence_range_},
                         {"confidence_window", confidence_window_},
                         {"measure",        measure_}}) {}

            /** @brief Write the equilibrium time parameters as equilibrium_time_params.json into rel_root_dir
             *
             * @param rel_root_dir Relative path to the project_root_dir for storing configuration files
             * @returns None
             */
            void write_to_file(const std::string &rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "equilibrium_time_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "equilibrium_time";
            }
            
            /** @brief Evaluates the equilibrium times by calling the respective Python functions and writes the results to file
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
                py::exec("import json");
                py::exec("from mcmctools.modes.equilibrium_time import equilibrium_time");
                py::exec("from mcmctools.loading.custom_function_support import get_custom_load_data_func");
                py::exec(("equilibrium_time(\
                    sample_size=" + std::to_string(sample_size) + ",\
                    number_of_steps=" + std::to_string(number_of_steps) + ",\
                    measure='" + measure + "',\
                    confidence_range=" + std::to_string(confidence_range) + ",\
                    confidence_window=" + std::to_string(confidence_window) + ",\
                    running_parameter='" + running_parameter + "',\
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
                return mcmc::simulation::MarkovChain(1, number_of_steps, 2 * sample_size, 0, "alternating");
            }

            std::vector<std::string> get_measures() {
                return std::vector<std::string>{measure};
            }

        private:
            uint sample_size;
            uint number_of_steps;
            double confidence_range;
            uint confidence_window;
            std::string measure;
        };

    }
}

#endif //MCMCSIMULATIONLIB_EQUILIBRIUM_HPP
