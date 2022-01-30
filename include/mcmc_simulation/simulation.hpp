#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>

// #include <boost/filesystem.hpp>

#include "util/mcmc_parameter_functions.hpp"
#include "markov_chain.hpp"

#include <param_helper/json.hpp>
#include <param_helper/filesystem.hpp>

#include "modes/from_file_preparation.hpp"

using json = nlohmann::json;

namespace mcmc
{
    namespace simulation
    {
        json::iterator get_running_parameter_iterator(json& params, std::vector<std::string> running_parameter_path);

        json update_running_parameter(const json params, std::vector<std::string> running_parameter_path, double new_running_parameter);

        /** @brief Helper class for preparing a MCMC simulation
         *
         * The class features different constructores allowing for running simulations based
         * on the same model or execution parameters and for simulations on a cluster or
         * locally based on configuration files.
         * 
         */
        template <typename SBP, typename EP=mcmc::mode::FromFilePreparation>
        class SimulationParameters : public param_helper::params::Parameters {
        public:
            // Constructor for loading from params
            explicit SimulationParameters(const json params_) : Parameters(params_), mode(EP::name()),
                                                                rel_data_path(get_entry<std::string>("rel_data_path")), // The remaining parameters are all optional
                                                                running_parameter_kind(get_entry<std::string>("running_parameter_kind", "None")),
                                                                running_parameter(get_entry<std::string>("running_parameter", "None")),
                                                                rp_intervals(get_entry<std::vector<double>>("rp_intervals", std::vector<double>{0.0}))
            {
                std::cout << "\n-- Setting up MCMC simulation --" << std::endl;

                // *** DIRECTORIES ***
                // Create folder in data directory if not present
                if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_data_path))
                {
                    // std::cout << "Create data directory" << std::endl;
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
                    // std::cout << "Load execution parameters from file based on given mode" << std::endl;
                    std::string execution_params_path = get_entry<std::string>("execution_params_path");
                    execution_params = std::make_unique<EP>(
                            param_helper::fs::read_parameter_file(execution_params_path, mode + "_params"));
                }

                // *** MEASURES ***
                // Set systembase measures <-> Introduce this as an error for usage of systembase with Simulation? In the sense that as soon you run a simulation with an execution mode, systembase shouldn't have any measures?
                if(!systembase_params->get_measures().empty())
                    std::cout << " -- Note that due to the usage of execution parameters, measures set by systembase parameters are ignored -- " << std::endl;
                auto execution_params_measures = execution_params->get_measures();
                systembase_params->set_measures(execution_params_measures);

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
                        std::cerr << "Running parameter kind not found" << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    running_parameter_path.push_back(running_parameter);
                }
            }

            /** @brief Prepare a simulation based on systembase parameters and execution parameters
             * 
             * @param systembase_params_ Instance of a systembase parameter class
             * @param execution_params_ Instance of an exeuction mode parameter class
             * @param rel_data_path_ Relative path to the project_dir of the simulation for storing the simulation data
             * @param running_parameter_kind_ Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
             * @param running_parameter_ Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
             */
            static SimulationParameters generate_simulation(
                    SBP& systembase_params_,
                    EP& execution_params_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                // Assert if EP = PrepareFromFile
                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {systembase_params_.name(), systembase_params_.get_json()},
                                {"execution_params", execution_params_.get_json()}
                        }
                );
            }

            /** @brief Prepare a simulation using the same execution parameters stored at rel_execution_params_path
             * 
             * The static constructor allows for running different simulations with the same exeuction parameters.
             * 
             * @param systembase_params_ Instance of a systembase parameter class
             * @param rel_execution_params_path_ Relative path to the project_dir of the simulation pointing to the execution params .json file
             * @param rel_data_path_ Relative path to the project_dir of the simulation for storing the simulation data
             * @param running_parameter_kind_ Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
             * @param running_parameter_ Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
             */
            static SimulationParameters generate_simulation(
                    SBP& systembase_params_,
                    const std::string& rel_execution_params_path_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                // Assert if rel_exeuction_params_path not found
                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {systembase_params_.name(), systembase_params_.get_json()},
                                {"execution_params_path", rel_execution_params_path_}
                        }
                );
            }

            /** @brief Prepare a simulation using the same execution parameters and systembase parameters stored at rel_systembase_params_path and rel_execution_params_path
             * 
             * The static constructor allows for running different simulations with the same execution parameters and systembase parameters. It is useful if other parameter ranges need to be covered.
             * 
             * @param rel_systembase_params_path_ Relative path to the project_dir of the simulation pointing to the sim_params.json file
             * @param rel_execution_params_path_ Relative path to the project_dir of the simulation pointing to the execution params .json file
             * @param rel_data_path_ Relative path to the project_dir of the simulation for storing the simulation data
             * @param running_parameter_kind_ Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
             * @param running_parameter_ Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
             */
            static SimulationParameters generate_simulation(
                    const std::string& rel_systembase_params_path_,
                    const std::string& rel_execution_params_path_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                // Check also for others? (in other constructors?), yes, also execution params <-> recheck with main constructor
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SBP::name(), true))
                {
                    std::cerr << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SBP::name() + "_path", rel_systembase_params_path_},
                                {"execution_params_path", rel_execution_params_path_}
                        }
                );
            }

            /** @brief Prepare a simulation that will be executed from file
             * 
             * ToDo: Explain what kind of EP need to be used in this case..
             * 
             * @param systembase_params_ Instance of a systembase parameter class
             * @param rel_data_path_ Relative path to the project_dir of the simulation for storing the simulation data
             * @param running_parameter_kind_ Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
             * @param running_parameter_ Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
             */
            static SimulationParameters prepare_simulation_from_file(
                    SBP& systembase_params_,
                    const std::string& rel_data_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                // Assert if EP != PrepareFromFile
                EP execution_params_;

                return SimulationParameters(
                        json {
                                {"rel_data_path", rel_data_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {systembase_params_.name(), systembase_params_.get_json()},
                                {"execution_params", execution_params_.get_json()} // Remove when writing to file??
                        }
                );
            }

            /** @brief Generate a simulation solely from file
             * 
             * Uses the given paths to load an entire simulation - It is important that the template parameters are in
             * concordance with the simulation parameter file.
             * If the rel_execution_params_path == "" the execution path will be loaded from the simulation parameter file
             * Otherwise the execution file will be loaded from the provided path
             * 
             * @param rel_sim_params_path_ Relative path to the project_dir of the simulation pointing to the sim_params.json file
             * @param rel_execution_params_path_ Relative path to the project_dir of the simulation pointing to the execution params .json file

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

            /** @brief Write the simulation parameters as sim_params.json to file.
             * 
             * @param rel_config_path Reltive path to the project_root define by param_helper::proj::set_relative_path_to_project_root_dir("../"). The file will be sotred in project_root/rel_config_path, respectively.
             */
            void write_to_file(std::string rel_config_path) {
                // Create folder in config directory if not present
                if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_config_path))
                {
                    // std::cout << "Create config directory" << std::endl;
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

            void update_system_params_by_new_running_parameter(double new_running_parameter) {
                auto expanded_parameters = build_expanded_raw_parameters().get_json();
                json new_system_params = update_running_parameter(
                        expanded_parameters, running_parameter_path, new_running_parameter);
                systembase_params = std::make_unique<SBP>(new_system_params[systembase_params->name()]);
            }

            const std::vector<std::string> get_measures() const
            {
                return systembase_params->get_measures();
            }

            static const std::string name()
            {
                return "sim_params";
            }

            std::vector<double> rp_intervals;
            std::string running_parameter;
            std::string rel_data_path;

            std::unique_ptr<SBP> systembase_params; // -> do these really have to be pointer?
            std::unique_ptr<EP> execution_params;

        private:
            template <typename, typename>
            friend class Simulation;

            std::string mode;
            std::string running_parameter_kind;
            std::vector<std::string> running_parameter_path;

            std::unique_ptr<MarkovChainParameters> markovchain_params;
        };

        /** @brief Class for performing the actual simulation as well as evaluation of a MCMC simulation
         * 
         */
        template <typename SBP, typename EP>
        class Simulation {
        public:
            
            /** @brief Constructor for setting up a simulation
             * 
             *  @param sp_: SimulationParameters containing all important parameters for the simulation
             */
            explicit Simulation(SimulationParameters<SBP, EP> &sp_) : sp(sp_)
            {}

            /** @brief Runs the MCMC simulation and generates .txt file containing all the measurements in the rel_data_path of the simulation parameters
             */
            void run() {
                std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

                if(sp.running_parameter == "None")
                    single_run(sp.mode);
                else
                {
                    for(auto rp_value : sp.rp_intervals)
                        single_run(sp.mode + "_" + sp.running_parameter + "=" + std::to_string(rp_value), rp_value);
                }
            }
            
            /** @brief Executes the respective Python functions for evaluating the MCMC simulation and stores the results in rel_results_dir
             *  
             *  For the evaluation to work, one need to enable Python in CMake and initialize Python by mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH) in the main function.
             * 
             *  @param rel_results_dir Relative path to sim_root_dir for storing the results
             *  @param sim_root_dir Relative path to the simulation directory (to the project_root_dir set by param_helper::proj::set_relative_path_to_project_root_dir("../"))
             */
            void eval(std::string rel_results_dir, std::string sim_root_dir="./")
            {
                std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

                sp.execution_params->evaluate(sp.rel_data_path, rel_results_dir, sim_root_dir, sp.running_parameter,
                    sp.rp_intervals, sp.build_expanded_raw_parameters().get_json()
                );
            }

        private:
            SimulationParameters<SBP, EP> &sp;

            void single_run(std::string filename, const double rp=0) {
                sp.initialize_simulation_parameters(rp);

                write_setting_file(sp.rel_data_path, filename);

                param_helper::fs::Fileos os (param_helper::proj::project_root() + sp.rel_data_path + "/" + filename  + ".dat");
                MarkovChain<SBP> mc(*sp.markovchain_params, *sp.systembase_params, os.get());
                mc.run();
            }

            // To get explicit informations about the particular run
            void write_setting_file(const std::string& rel_path, const std::string &filename) const
            {
                auto raw_parameters = sp.build_expanded_raw_parameters();
                raw_parameters.write_to_file(rel_path, filename);
            }
        };

    }
}

#endif
