//
// Created by lukas on 14.09.20.
//

#ifndef MCMCSIMULATIONLIB_EQUILIBRIUM_HPP
#define MCMCSIMULATIONLIB_EQUILIBRIUM_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "mcmc_simulation/markov_chain.hpp"
#include "execution/python_integration.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace mode {

        /** @brief Prepares the computation of the necessary number of Monte Carlo sweeps for the system to in equilibrium
         *
         * This class defines necessary parameters for running a MCMC simulation
         * for determining the number of Monte Carlo sweeps the system needs to be in equilibrium.
         * Note that some of the parameters are only needed for the respective evaluation which
         * is executed afterwards in Python.
         */
        class EquilibriumTimeParameters : public param_helper::params::Parameters {
        public:
            explicit EquilibriumTimeParameters(const json params_) : Parameters(params_) {
                sample_size = get_entry<uint>("sample_size", 100);
                number_of_steps = get_entry<uint>("number_of_steps", 1000);
                confidence_range = get_entry<double>("confidence_range", 0.1);
                confidence_window = get_entry<uint>("confidence_window", 10);
                measure = get_entry<std::string>("measure", "Mean");
            }

            /** @brief Constructor for defining all important parameters
             *
             * @param sample_size_ Number of Monte Carlo sweeps between measurements (autocorrelation time)
             * @param number_of_steps_ Total number of measurements
             * @param confidence_range_ Number of Monte Carlo sweeps before starting with the first measurement
             * @param confidence_window_ Measures to be made during the simulation
             * @param measure_ Measures which should be computed afterwards in Python.
             *  Note that for this work the configurations have to be stored as well which can be achieved
             *  by adding "Config" to measures
             */
            EquilibriumTimeParameters(
                    uint sample_size_,
                    uint number_of_steps_,
                    double confidence_range_,
                    uint confidence_window_,
                    std::string measure_="Mean"
            ) : EquilibriumTimeParameters(
                    json{{"sample_size",    sample_size_},
                         {"number_of_steps", number_of_steps_},
                         {"confidence_range", confidence_range_},
                         {"confidence_window", confidence_window_},
                         {"measure",        measure_}}) {}

            /** @brief Write the equilibrium time parameters as equilibrium_time_params.json into root_dir
             *
             * @param root_dir Absolute path to the output directory
             * @returns None
             */
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
            
            /** @brief Evaluate equilibrium times by calling the respective Python functions and writes the results to file
             *
             * @param x ...Todo
             * @returns None
             */
            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir, const std::string sim_root_dir,
                const std::string running_parameter="None", const std::vector<double>& rp_intervals=std::vector<double>{0.0}, const json simparams_json={})
            {
                #ifdef RUN_WITH_PYTHON_BACKEND
                py::exec("import json");
                py::exec("from mcmctools.modes.equilibrium_time import equilibrium_time");
                py::exec(("equilibrium_time(\
                    sample_size=" + std::to_string(sample_size) + ",\
                    number_of_steps=" + std::to_string(number_of_steps) + ",\
                    measure='" + measure + "',\
                    confidence_range=" + std::to_string(confidence_range) + ",\
                    confidence_window=" + std::to_string(confidence_window) + ",\
                    running_parameter='" + running_parameter + "',\
                    rp_keys=" + json(rp_intervals).dump() + ",\
                    rel_data_dir='" + rel_data_dir + "',\
                    rel_results_dir='" + rel_results_dir + "',\
                    sim_base_dir='" + param_helper::fs::prfs::project_root() + sim_root_dir + "',\
                    fma=fma)").c_str());
                #endif
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
