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

            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir, const std::string sim_root_dir,
                const std::string running_parameter="None", const double rp_minimum=0.0,
                const double rp_maximum=0.0, const int rp_number=0, const json simparams_json={})
            {
                #ifdef RUN_WITH_PYTHON_BACKEND
                py::exec("from mcmctools.modes.correlation_time import correlation_time");
                py::exec("from mcmctools.utils.utils import retrieve_rp_keys");
                py::exec(("correlation_time(\
                    minimum_sample_size=" + std::to_string(minimum_sample_size) + ",\
                    maximum_correlation_time=" + std::to_string(maximum_correlation_time) + ",\
                    measure='" + measure + "',\
                    running_parameter='" +running_parameter + "',\
                    rp_keys=retrieve_rp_keys(running_parameter='" + running_parameter + "',\
                        rp_minimum=" + std::to_string(rp_minimum) + ",\
                        rp_maximum=" + std::to_string(rp_maximum) + ",\
                        rp_number=" + std::to_string(rp_number) + "),\
                    rel_data_dir='" + rel_data_dir + "',\
                    rel_results_dir='" + rel_results_dir + "',\
                    sim_base_dir='" + param_helper::fs::prfs::project_root() + sim_root_dir + "',\
                    fma=fma)").c_str());
                #endif
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
                    std::cout << " -- Looking for equilibrium time for rp=" << rp << " in equilibrium_time_results.json --" /*(in: "
                              << param_helper::fs::prfs::get_path_to(equilibrium_time_rel_results_path)
                              << ") --"*/
                              << std::endl;
                    equilibrium_time = param_helper::params::entry_by_key<uint>(
                            equilibrium_time_results["EquilibriumTime"], rp_key);
                }

                return std::make_unique<mcmc::simulation::MarkovChainParameters>(
                        1,
                        minimum_sample_size + maximum_correlation_time,
                        1,
                        equilibrium_time,
                        "hot");
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
        };
    }
}

#endif //MAIN_CORRELATION_TIME_HPP
