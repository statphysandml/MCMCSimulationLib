#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>

// #include <boost/filesystem.hpp>

#include "util/mcmc_parameter_functions.hpp"
#include "markov_chain.hpp"

#include "param_helper/json.hpp"
#include "param_helper/filesystem.hpp"

using json = nlohmann::json;

namespace mcmc
{
    namespace simulation
    {
        json::iterator get_running_parameter_iterator(json& params, std::vector<std::string> running_parameter_path);

        json update_running_parameter(const json params, std::vector<std::string> running_parameter_path, double new_running_parameter);

        template <typename SBP, typename EP>
        class SimulationParameters : public param_helper::params::Parameters {
        public:
            // Constructor for loading from params
            explicit SimulationParameters(const json params_) : Parameters(params_), mode(EP::name()),
                rel_data_path(get_entry<std::string>("rel_data_path")), // The remaining parameters are all optional
                running_parameter_kind(get_entry<std::string>("running_parameter_kind", "None")),
                running_parameter(get_entry<std::string>("running_parameter", "None")),
                rp_minimum(get_entry<double>("rp_minimum", 0)),
                rp_maximum(get_entry<double>("rp_maximum", 0)),
                rp_number(get_entry<int>("rp_number", 0))
            {
                // *** DIRECTORIES ***
                // Create folder in data directory if not present
                if(!param_helper::fs::direxists(param_helper::fs::prfs::project_root() + rel_data_path))
                {
                    std::cout << "Create data directory" << std::endl;
                    param_helper::fs::generate_directory_if_not_present(rel_data_path);
                }

                // *** SYSTEM BASE PARAMETERS *** (default, from file, from json object)
                systembase_params = std::make_unique<SBP>(
                        mcmc::util::generate_parameter_class_json<SimulationParameters<SBP, EP>, SBP> (*this, systembase_params->name()));

                // *** EXECUTION PARAMETERS ***
                // Load execution parameters from params object -> enables to use
                if(haskey("execution_params")) {
                    execution_params = std::make_unique<EP>(get_entry<json>("execution_params"));
                }
                // Load execution parameters from file
                else {
                    std::cout << "Load execution parameters from file based on given mode" << std::endl;
                    std::string execution_params_path = get_entry<std::string>("execution_params_path");
                    execution_params = std::make_unique<EP>(
                            param_helper::fs::read_parameter_file(execution_params_path, mode + "_params"));
                }

                // *** MEASURES ***
                // Temporarily replace measures if wanted
                auto &execution_params_measures = execution_params->get_measures();
                if(!execution_params_measures.empty())
                    systembase_params->update_measures(execution_params_measures);

                // *** RUNNING PARAMETER ***
                if(running_parameter != "None")
                {
                    // Construct running parameter path for later simulations
                    auto expanded_parameters = build_expanded_raw_parameters();
                    // std::cout << expanded_parameters << std::endl;
                    running_parameter_path = std::vector<std::string> {};
                    auto running_parameter_kind_path_found = param_helper::params::construct_parameter_path(expanded_parameters.get_json(), running_parameter_kind, running_parameter_path);
                    if(!running_parameter_kind_path_found)
                    {
                        std::cout << "Running parameter kind not found" << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    running_parameter_path.push_back(running_parameter);
                }
            }

            /*
             * Generates simulation based on systembase parameters and execution parameters that need to be defined before hand
             *
             */
            static SimulationParameters generate_simulation(
                    SBP& systembase_params_,
                    EP& execution_params_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const double& rp_minimum_=0,
                    const double& rp_maximum_=0,
                    const double& rp_number_=0
            )
            {
                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_minimum", rp_minimum_},
                                {"rp_maximum", rp_maximum_},
                                {"rp_number", rp_number_},
                                {systembase_params_.name(), systembase_params_.get_json()},
                                {"execution_params", execution_params_.get_json()}
                        }
                );
            }

            /*
             * Uses the given paths to load an entire simulation - It is important that the template parameters are in
             * concordance with the simulation parameter file.
             * If the rel_execution_params_path == "" the execution path will be loaded from the simulation parameter file
             * Otherwise the execution file will be loaded from the provided path
             */
            static SimulationParameters generate_simulation_from_file(
                    const std::string& rel_sim_params_path_,
                    const std::string& rel_execution_params_path="")
            {
                // Prechecks if there exists a file at rel_sim_params_path
                auto params_ = param_helper::fs::read_parameter_file(rel_sim_params_path_, name());
                // Enables the possiblity to run with an execution parameter file that is stored somewhere else in different execution modes
                if(rel_execution_params_path != "")
                    params_["execution_params_path"] = rel_execution_params_path;
                return SimulationParameters<SBP, EP> (params_);
            }

            /*
             * One can run different simulations for the same execution file that is stored at rel_execution_params_path
             */
            static SimulationParameters generate_simulation_for_execution_params_from_path(
                    SBP& systembase_params_,
                    const std::string& rel_execution_params_path_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const double& rp_minimum_=0,
                    const double& rp_maximum_=0,
                    const double& rp_number_=0
            )
            {
                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_minimum", rp_minimum_},
                                {"rp_maximum", rp_maximum_},
                                {"rp_number", rp_number_},
                                {systembase_params_.name(), systembase_params_.get_json()},
                                {"execution_params_path", rel_execution_params_path_}
                        }
                );
            }

            /*
             * One can run different simulations for the same execution file and the same simulation parameters
             * the files are stored at rel_systembase_params_path and at rel_execution_params_path
             */
            static SimulationParameters generate_custom_simulation(
                    const std::string& rel_systembase_params_path_,
                    const std::string& rel_execution_params_path_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const double rp_minimum_=0.0,
                    const double rp_maximum_=0.0,
                    const int rp_number_=0.0
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SBP::name(), true))
                {
                    std::cout << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_minimum", rp_minimum_},
                                {"rp_maximum", rp_maximum_},
                                {"rp_number", rp_number_},
                                {SBP::name() + "_path", rel_systembase_params_path_},
                                {"execution_params_path", rel_execution_params_path_}
                        }
                );
            }

            void initialize_simulation_parameters(const double rp=0)
            {
                if(running_parameter != "None") {
                    markovchain_params = execution_params->generate_markovchain_params(running_parameter, rp);
                    update_system_params_by_new_running_parameter(rp);
                }
                else {
                    markovchain_params = execution_params->generate_markovchain_params();
                }
            }

            void write_to_file(std::string rel_config_path) {
                // Create folder in config directory if not present
                if(!param_helper::fs::direxists(param_helper::fs::prfs::project_root() + rel_config_path))
                {
                    std::cout << "Create config directory" << std::endl;
                    param_helper::fs::generate_directory_if_not_present(rel_config_path);
                }

                std::string execution_params_path = get_entry<std::string>("execution_params_path", rel_config_path); // First one enables loading it from somewhere else!
                execution_params->write_to_file(execution_params_path);

                std::string systembase_params_path = get_entry<std::string>(systembase_params->name() + "_path", rel_config_path);
                systembase_params->write_to_file(systembase_params_path);

                json execution_params_ = get_entry<json>("execution_params");
                delete_entry("execution_params");

                json systembase_params_ = systembase_params->get_json();
                delete_entry(systembase_params->name());

                Parameters::write_to_file(rel_config_path, name());

                add_entry("execution_params", execution_params_);
                add_entry(systembase_params->name(), systembase_params_);

            }

            Parameters build_expanded_raw_parameters()
            {
                Parameters parameters = Parameters::create_by_params(params);
                parameters.add_entry(systembase_params->name(), systembase_params->build_expanded_raw_parameters().get_json());
                parameters.add_entry("execution_params", execution_params->build_expanded_raw_parameters().get_json());
                return parameters;
            }

            void print() {
                systembase_params->print_something();
            }

            void update_system_params_by_new_running_parameter(double new_running_parameter) {
                auto expanded_parameters = build_expanded_raw_parameters().get_json();
                json new_system_params = update_running_parameter(
                        expanded_parameters, running_parameter_path, new_running_parameter);
                systembase_params = std::make_unique<SBP>(new_system_params[systembase_params->name()]);
            }

            static const std::string name()
            {
                return "sim_params";
            }

        private:
            template <typename, typename>
            friend class Simulation;

            std::string mode;
            std::string rel_data_path;
            std::string running_parameter_kind;
            std::string running_parameter;
            double rp_minimum;
            double rp_maximum;
            int rp_number;

            std::unique_ptr<SBP> systembase_params; // -> do these really have to be pointer?
            std::unique_ptr<EP> execution_params;

            std::vector<std::string> running_parameter_path;

            std::unique_ptr<MarkovChainParameters> markovchain_params;
        };


        template <typename SBP, typename EP>
        class Simulation {
        public:
            explicit Simulation(SimulationParameters<SBP, EP> &mp_) : mp(mp_)
            {}

            void run() {
                if(mp.running_parameter == "None")
                    single_run(mp.mode);
                else
                {
                    for (double rp = mp.rp_minimum; rp <= mp.rp_maximum; rp += (mp.rp_maximum - mp.rp_minimum) / mp.rp_number) {
                        single_run(mp.mode + "_" + mp.running_parameter + "=" + std::to_string(rp), rp);
                    }
                }
            }

            void single_run(std::string filename, const double rp=0) {
                mp.initialize_simulation_parameters(rp);

                write_setting_file(mp.rel_data_path, filename);

                param_helper::fs::Fileos os (param_helper::fs::prfs::project_root() + mp.rel_data_path + "/" + filename  + ".dat");
                MarkovChain<SBP> mc(*mp.markovchain_params, *mp.systembase_params, os.get());
                mc.run();
            }

        private:
            SimulationParameters<SBP, EP> &mp;

            // To get explicit informations about the particular run
            void write_setting_file(const std::string& rel_path, const std::string &filename) const
            {
                auto raw_parameters = mp.build_expanded_raw_parameters();
                raw_parameters.write_to_file(rel_path, filename);
            }
        };

    }
}

#endif