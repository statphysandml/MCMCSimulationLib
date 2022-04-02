#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>
#include <chrono>
#include <thread>

// #include <boost/filesystem.hpp>

#include "util/mcmc_parameter_functions.hpp"
#include "markov_chain.hpp"

#include <param_helper/json.hpp>
#include <param_helper/filesystem.hpp>

#include "modes/from_file_preparation.hpp"
#include "measure_processors/readable_measure.hpp"

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
         * on the same system, execution and measure parameters, for simulations from configuration files.
         * 
         * The static constructor Simulation::generate_simulation is overloaded to allow for various combinations
         * of parameters defined within in C++ or loaded from file. The list of possible parameters is as follows:
         * 
         * <b>Parameters</b>:
         *     - <b>system_base</b> Object of a systembase parameter class
         *     - <b>execution_mode_</b> Object of an exeuction mode parameter class
         *     - <b>measurement_</b> Object of a measure parameter class handling gathering and writing the simulation data to file
         *     - <b>running_parameter_kind_</b> Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
         *     - <b>running_parameter_</b> Name of the running parameter (default: "None")
         *     - <b>rp_intervals_</b> List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
         *     - <b>rel_systembase_params_path_</b> Relative path to the project_dir of the simulation pointing to the sim_params.json file
         *     - <b>rel_execution_mode_path_</b> Relative path to the project_dir of the simulation pointing to the execution_mode.json file
         *     - <b>rel_measurement_path_</b> Relative path to the project_dir of the simulation pointing to the readable_measurement.json file
         * 
         * The respective constructors and constructors for generating simulations entirely from file are listed below.
         */
        /* template <typename SBP, typename EP=mcmc::mode::FromFilePreparation, typename MS=mcmc::measures::ReadableMeasureParameters>
        class Simulation : public param_helper::params::Parameters {
        public: */

        /** @brief Class for performing the actual simulation as well as evaluation of a MCMC simulation
         * 
         */
        template <typename SB, typename EP=mcmc::mode::FromFilePreparation, typename MS=mcmc::measures::ReadableMeasure>
        class Simulation : public param_helper::params::Parameters {
        public:
            // Constructor for loading from params
            explicit Simulation(const json params_) : Parameters(params_), mode(EP::name()), measure_system(MS::name()),// The remaining parameters are all optional
                                                                running_parameter_kind(get_entry<std::string>("running_parameter_kind", "None")),
                                                                running_parameter(get_entry<std::string>("running_parameter", "None")),
                                                                rp_intervals(get_entry<std::vector<double>>("rp_intervals", std::vector<double>{0.0}))
            {
                std::cout << "\n-- Setting up MCMC simulation --" << std::endl;

                // *** SYSTEM BASE PARAMETERS *** (default, from file, from json object)
                systembase = SB(mcmc::util::generate_parameter_class_json<Simulation<SB, EP, MS>, SB> (*this, SB::name()));

                // *** EXECUTION PARAMETERS ***
                // Load execution parameters from params object -> enables to use
                if(haskey("execution_mode")) {
                    execution_mode = EP(get_entry<json>("execution_mode"));
                }
                    // Load execution parameters from file
                else {
                    // std::cout << "Load execution parameters from file based on given mode" << std::endl;
                    std::string execution_mode_path = get_entry<std::string>("execution_mode_path");
                    execution_mode = EP(param_helper::fs::read_parameter_file(execution_mode_path, mode + "_params"));
                }

                // *** MEASURES ***
                // Load measurements parameters
                if(haskey("measurement")) {
                    measurement = MS(get_entry<json>("measurement"));
                }
                    // Load measure parameters from file
                else {
                    // std::cout << "Load measure parameters from file based on given measure_system" << std::endl;
                    std::string measurement_path = get_entry<std::string>("measurement_path");
                    measurement = MS(param_helper::fs::read_parameter_file(measurement_path, measure_system + "_params"));
                }

                // Set systembase measures <-> Introduce this as an error for usage of systembase with Simulation? In the sense that as soon you run a simulation with an execution mode, systembase shouldn't have any measures?
                if(!systembase.measure_names().empty())
                    std::cout << " -- Note that due to the usage of execution parameters, measures set by systembase parameters are ignored -- " << std::endl;
                auto execution_mode_measures = execution_mode.get_measures();
                systembase.set_measures(execution_mode_measures);

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

            /* Constructors with measurement_ as an object */

            /** @brief Simulation based on systembase parameters, execution parameters and measure parameters */
            static Simulation generate_simulation(
                    SB& systembase_,
                    EP& execution_mode_,
                    MS& measurement_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                return Simulation(
                        json {
                                {"measurement", measurement_.get_json()},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name(), systembase_.get_json()},
                                {"execution_mode", execution_mode_.get_json()}
                        }
                );
            }

            /** @brief Simulation based on execution parameters and measure parameters, where systembase parameters are loaded
             * from rel_systembase_params_path_ */
            static Simulation generate_simulation(
                    const std::string& rel_systembase_params_path_,
                    EP& execution_mode_,
                    MS& measurement_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SB::name(), true))
                {
                    std::cerr << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement", measurement_.get_json()},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name() + "_path", rel_systembase_params_path_},
                                {"execution_mode", execution_mode_.get_json()}
                        }
                );
            }

            /** @brief Simulation based on systembase parameters and measure parameters, where execution parameters are
             * loaded from rel_execution_mode_path_
             * */
            static Simulation generate_simulation(
                    SB& systembase_,
                    const std::string& rel_execution_mode_path_,
                    MS& measurement_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path_, SB::name(), true))
                {
                    std::cerr << "Execution mode parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement", measurement_.get_json()},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name(), systembase_.get_json()},
                                {"execution_mode_path", rel_execution_mode_path_}
                        }
                );
            }

            /** @brief Simulation based on measure parameters, where systembase parameters are loaded from rel_systembase_params_path_
             * and execution parameters from rel_execution_mode_path_
             */
            static Simulation generate_simulation(
                    const std::string& rel_systembase_params_path_,
                    const std::string& rel_execution_mode_path_,
                    MS& measurement_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SB::name(), true))
                {
                    std::cerr << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path_, SB::name(), true))
                {
                    std::cerr << "Execution mode parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement", measurement_.get_json()},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name() + "_path", rel_systembase_params_path_},
                                {"execution_mode_path", rel_execution_mode_path_}
                        }
                );
            }

            /* Constructors with measurement_ loaded from file */

            /** @brief Simulation based on systembase parameters and execution parameters, where measure parameters are loaded
             * from rel_measurement_path_
             */
            static Simulation generate_simulation(
                    SB& systembase_,
                    EP& execution_mode_,
                    const std::string& rel_measurement_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path_, SB::name(), true))
                {
                    std::cerr << "Measurement processor parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement_path", rel_measurement_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name(), systembase_.get_json()},
                                {"execution_mode", execution_mode_.get_json()}
                        }
                );
            }

            /** @brief Simulation based on execution parameters, where systembase parameters are loaded from
             * rel_systembase_params_path_ and measure parameters from rel_measurement_path_
             */
            static Simulation generate_simulation(
                    const std::string& rel_systembase_params_path_,
                    EP& execution_mode_,
                    const std::string& rel_measurement_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SB::name(), true))
                {
                    std::cerr << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path_, SB::name(), true))
                {
                    std::cerr << "Measurement processor parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement_path", rel_measurement_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name() + "_path", rel_systembase_params_path_},
                                {"execution_mode", execution_mode_.get_json()}
                        }
                );
            }

            /** @brief Simulation based on systembase parameters, where execution parameters are loaded from
             * rel_execution_mode_path_ and measure parameters from rel_measurement_path_
             */
            static Simulation generate_simulation(
                    SB& systembase_,
                    const std::string& rel_execution_mode_path_,
                    const std::string& rel_measurement_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path_, SB::name(), true))
                {
                    std::cerr << "Execution mode parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path_, SB::name(), true))
                {
                    std::cerr << "Measurement processor parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement_path", rel_measurement_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name(), systembase_.get_json()},
                                {"execution_mode_path", rel_execution_mode_path_}
                        }
                );
            }

            /** @brief Simulation, where systembase parameters are loaded from rel_systembase_params_path_,
             * execution parameters from rel_execution_mode_path_ and measure parameters from rel_measurement_path_
             */
            static Simulation generate_simulation(
                    const std::string& rel_systembase_params_path_,
                    const std::string& rel_execution_mode_path_,
                    const std::string& rel_measurement_path_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path_, SB::name(), true))
                {
                    std::cerr << "Systembase parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path_, SB::name(), true))
                {
                    std::cerr << "Execution mode parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path_, SB::name(), true))
                {
                    std::cerr << "Measurement processor parameters have not been found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                return Simulation(
                        json {
                                {"measurement_path", rel_measurement_path_},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name() + "_path", rel_systembase_params_path_},
                                {"execution_mode_path", rel_execution_mode_path_}
                        }
                );
            }

            /** @brief Prepare a simulation that will be executed from file
             * 
             * @param systembase_params_ Object of a systembase parameter class
             * @param measurement_ Object of a measure parameter class handling gathering and writing the simulation data to file
             * @param running_parameter_kind_ Parent parameter module name, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: "None")
             * @param running_parameter_ Name of the running parameter (default: "None")
             * @param rp_intervals_ List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: std::vector<double>{0.0})
             */
            static Simulation prepare_simulation_from_file(
                    SB& systembase_,
                    MS& measurement_,
                    const std::string& running_parameter_kind_="None",
                    const std::string& running_parameter_="None",
                    const std::vector<double>& rp_intervals_=std::vector<double>{0.0}
            )
            {
                EP execution_mode_;

                return Simulation(
                        json {
                                {"measurement", measurement_.get_json()},
                                {"running_parameter_kind", running_parameter_kind_},
                                {"running_parameter", running_parameter_},
                                {"rp_intervals", rp_intervals_},
                                {SB::name(), systembase_.get_json()},
                                {"execution_mode", execution_mode_.get_json()}
                        }
                );
            }

            /** @brief Generate a simulation solely from file
             *  
             * Uses the given paths to load an entire simulation - It is important that the template parameters are in
             * concordance with the simulation parameter file.
             * 
             * @param rel_sim_params_path_ Relative path to the project_dir of the simulation pointing to the sim_params.json file
             * @param rel_execution_mode_path_ Relative path to the project_dir of the simulation pointing to the execution params .json file. if
             *  no path is provided, the default path is the one provided in sim_params.json
             * @param rel_measurement_path_ Relative path to the project_dir of the simulation pointing to the measurement processing params .json file. if
             *  no path is provided, the default path is the one provided in sim_params.json.
             */
            static Simulation generate_simulation_from_file(
                    const std::string& rel_sim_params_path_,
                    const std::string& rel_execution_mode_path="",
                    const std::string& rel_measurement_path="")
            {
                // Prechecks if there exists a file at rel_sim_params_path
                auto params_ = param_helper::fs::read_parameter_file(rel_sim_params_path_, name());
                // Enables the possiblity to run with an execution parameter file that is stored somewhere else in different execution modes
                if(rel_execution_mode_path != "")
                    params_["execution_mode_path"] = rel_execution_mode_path;
                if(rel_measurement_path != "")
                    params_["measurement_path"] = rel_measurement_path;
                return Simulation<SB, EP, MS> (params_);
            }

            void initialize_simulation_parameters(const double rp=0)
            {
                if(running_parameter != "None") {
                    markov_chain = execution_mode.generate_markov_chain(running_parameter, rp);
                    update_system_params_by_new_running_parameter(rp);
                }
                else {
                    markov_chain = execution_mode.generate_markov_chain();
                }
            }

            /** @brief Write the simulation parameters as sim_params.json to file.
             * 
             * @param rel_config_path Reltive path to the project_root define by param_helper::proj::set_relative_path_to_project_root_dir("../"). The file will be sotred in project_root/rel_config_path, respectively.
             */
            void write_to_file(const std::string rel_config_path) {
                // Create folder in config directory if not present
                if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_config_path))
                {
                    // std::cout << "Create config directory" << std::endl;
                    param_helper::fs::generate_directory_if_not_present(rel_config_path);
                }

                std::string execution_mode_path = get_entry<std::string>("execution_mode_path", rel_config_path); // First one enables loading it from somewhere else!
                execution_mode.write_to_file(execution_mode_path);

                std::string measurement_path = get_entry<std::string>("measurement_path", rel_config_path); // First one enables loading it from somewhere else!
                measurement.write_to_file(measurement_path);

                std::string systembase_params_path = get_entry<std::string>(SB::name() + "_path", rel_config_path);
                systembase.write_to_file(systembase_params_path);

                json execution_mode_ = get_entry<json>("execution_mode");
                delete_entry("execution_mode");

                json measurement_ = get_entry<json>("measurement");
                delete_entry("measurement");

                json systembase_params_ = systembase.get_json();
                delete_entry(SB::name());

                Parameters::write_to_file(rel_config_path, name());

                add_entry("execution_mode", execution_mode_);
                add_entry("measurement", measurement_);
                add_entry(SB::name(), systembase_params_);

            }

            Parameters build_expanded_raw_parameters() const
            {
                Parameters parameters = Parameters::create_by_params(params);
                parameters.add_entry(SB::name(), systembase.build_expanded_raw_parameters().get_json());
                parameters.add_entry("execution_mode", execution_mode.build_expanded_raw_parameters().get_json());
                parameters.add_entry("measurement", measurement.build_expanded_raw_parameters().get_json());
                return parameters;
            }

            void update_system_params_by_new_running_parameter(double new_running_parameter) {
                auto expanded_parameters = build_expanded_raw_parameters().get_json();
                json new_system_params = update_running_parameter(
                        expanded_parameters, running_parameter_path, new_running_parameter);
                systembase = SB(new_system_params[SB::name()]);
            }

            const std::vector<std::string> get_measures() const
            {
                return systembase.get_measures();
            }

            static const std::string name()
            {
                return "sim_params";
            }
            
            /** @brief Constructor for setting up a simulation
             * 
             *  @param sp_: Simulation containing all important parameters for the simulation
             */
            /* explicit Simulation(Simulation<SBP, EP, MS> &sp_) : sp(sp_)
            {} */

            /** @brief Runs the MCMC simulation and generates .txt file containing all the measurements in the rel_data_path of the simulation parameters
             */
            void run() {
                std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

                if(running_parameter == "None")
                    single_run();                    
                else
                {
                    for(auto rp_value : rp_intervals)
                        single_run(rp_value);
                }
            }
            
            /** @brief Executes the respective Python functions for evaluating the MCMC simulation and stores the results in rel_results_dir
             *  
             *  For the evaluation to work, one needs to enable Python in CMake and initialize Python by mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH) in the main function.
             * 
             *  @param rel_results_dir Relative path to the project_root_dir for storing the results
             */
            void eval(std::string rel_results_dir)
            {
                std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

                execution_mode.evaluate(measurement.get_rel_data_dir(), rel_results_dir, running_parameter,
                    rp_intervals, build_expanded_raw_parameters().get_json()
                );

                // Sleep for a short moment to be able to directly use the simulation results in a subsequent Monte Carlo simulation.
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1s);
            }

            std::vector<double> rp_intervals;
            std::string running_parameter;

            std::string mode;
            std::string measure_system;
            std::string running_parameter_kind;
            std::vector<std::string> running_parameter_path;

        private:
            SB systembase;
            EP execution_mode;
            MS measurement;

            MarkovChain markov_chain;

            void single_run(const double rp=0) {
                initialize_simulation_parameters(rp);
                measurement.initialize_run(mode, running_parameter, rp);

                write_setting_file(measurement.get_rel_data_dir(), measurement.get_setting_filename());
                
                markov_chain.run<MS, SB>(measurement, systembase);
            }

            // To get explicit informations about the particular run
            void write_setting_file(const std::string &rel_path, const std::string &filename) const
            {
                auto raw_parameters = build_expanded_raw_parameters();
                raw_parameters.write_to_file(rel_path, filename);
            }
        };

    }
}

#endif
