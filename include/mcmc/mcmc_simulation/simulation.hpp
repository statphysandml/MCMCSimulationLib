#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>
#include <chrono>
#include <thread>

// #include <boost/filesystem.hpp>

#include <mcmc/mcmc_simulation/util/mcmc_parameter_functions.hpp>
#include <mcmc/mcmc_simulation/markov_chain.hpp>

#include <nlohmann/json.hpp>
#include <param_helper/filesystem.hpp>

#include <mcmc/modes/from_file_preparation.hpp>
#include <mcmc/measure_processors/readable_measure.hpp>

using json = nlohmann::json;

namespace mcmc::simulation {
    json::iterator get_running_parameter_iterator(json& params, std::vector<std::string> running_parameter_path);

    json update_running_parameter(const json params, std::vector<std::string> running_parameter_path, double new_running_parameter);

    /** @brief Class for performing the MCMC simulation as well as for
     * computing simulation results based on different evaluation modes.
     *
     * The class features different constructores allowing for running
     * simulations based on the same system, execution and measure
     * parameters from configuration files.
     *
     * The static constructor Simulation::generate_simulation is overloaded
     * to allow for various combinations of parameters defined in C++ or
     * loaded from file. The list of possible parameters is as follows:
     * 
     * <b>Parameters</b>:
     *     - <b>systembase</b> Object of a systembase parameter class
     *     - <b>execution_mode</b> Object of an execution mode parameter class
     *     - <b>measurement</b> Object of a measure parameter class handling gathering and writing the simulation data to file
     *     - <b>running_parameter_kind</b> Parent configuration module key, providing the program with the necessary information of where to find the running parameter. If none is given, the simulation is only executed for the original set of parameters (default: ``"None"``)
     *     - <b>running_parameter</b> Name of the running parameter (default: ``"None"``)
     *     - <b>rp_intervals</b> List of values for the running parameter. A MCMC simulation is executed for each of these values one by one (default: ``std::vector<double>{0.0}``)
     *     - <b>rel_systembase_params_path</b> Relative path to the ``project_root_dir`` of the simulation pointing to the ``systembase_params.json`` file
     *     - <b>rel_execution_mode_path</b> Relative path to the ``project_root_dir`` of the simulation pointing to the execution mode params ``.json`` file
     *     - <b>rel_measurement_path</b> Relative path to the ``project_root_dir`` of the simulation pointing to the measurement processing params ``.json`` file
     * 
     * Respective constructors and constructors for generating simulations
     * entirely from file are listed below.
     */
    template <typename SB, typename EP=mcmc::mode::FromFilePreparation, typename MS=mcmc::measures::ReadableMeasure>
    class Simulation : public param_helper::params::Parameters {
    public:
        // Constructor for loading from params
        explicit Simulation(const json params) : Parameters(params), mode_(EP::name()), measure_system_(MS::name()),// The remaining parameters are all optional
            running_parameter_kind_(get_entry<std::string>("running_parameter_kind", "None")),
            running_parameter_(get_entry<std::string>("running_parameter", "None")),
            rp_intervals_(get_entry<std::vector<double>>("rp_intervals", std::vector<double>{0.0}))
        {
            std::cout << "\n-- Setting up MCMC simulation --" << std::endl;

            // *** SYSTEM BASE PARAMETERS *** (default, from file, from json object)
            systembase_ = SB(mcmc::util::generate_parameter_class_json<Simulation<SB, EP, MS>, SB> (*this, SB::name()));

            // *** EXECUTION PARAMETERS ***
            // Load execution parameters from params object -> enables to use
            if(haskey("execution_mode")) {
                execution_mode_ = EP(get_entry<json>("execution_mode"));
            }
                // Load execution parameters from file
            else {
                // std::cout << "Load execution parameters from file based on given mode" << std::endl;
                std::string execution_mode_path = get_entry<std::string>("execution_mode_path");
                execution_mode_ = EP(param_helper::fs::read_parameter_file(execution_mode_path, mode_ + "_params"));
            }

            // *** MEASURES ***
            // Load measurements parameters
            if(haskey("measurement")) {
                measurement_ = MS(get_entry<json>("measurement"));
            }
                // Load measure parameters from file
            else {
                // std::cout << "Load measure parameters from file based on given measure_system" << std::endl;
                std::string measurement_path = get_entry<std::string>("measurement_path");
                measurement_ = MS(param_helper::fs::read_parameter_file(measurement_path, measure_system_ + "_params"));
            }

            // Set systembase measures <-> Introduce this as an error for usage of systembase with Simulation? In the sense that as soon you run a simulation with an execution mode, systembase shouldn't have any measures?
            if(!systembase_.measure_names().empty())
                std::cout << " -- Note that due to the usage of execution parameters, measures set by systembase parameters are ignored -- " << std::endl;
            auto execution_mode_measures = execution_mode_.get_measures();
            systembase_.set_measures(execution_mode_measures);

            // *** RUNNING PARAMETER ***
            if(running_parameter_ != "None")
            {
                // Construct running parameter path for later simulations
                auto expanded_parameters = build_expanded_raw_parameters();
                // std::cout << expanded_parameters << std::endl;
                running_parameter_path_ = std::vector<std::string> {};
                auto running_parameter_kind_path_found = param_helper::params::construct_parameter_path(expanded_parameters.get_json(), running_parameter_kind_, running_parameter_path_);
                if(!running_parameter_kind_path_found)
                {
                    std::cerr << "Running parameter kind not found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                running_parameter_path_.push_back(running_parameter_);
            }
        }

        /* Constructors with measurement as an object */

        /** @brief Simulation based on systembase parameters, execution
         * parameters and measure parameters. */
        static Simulation generate_simulation(
                SB& systembase,
                EP& execution_mode,
                MS& measurement,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            return Simulation(
                    json {
                            {"measurement", measurement.get_json()},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name(), systembase.get_json()},
                            {"execution_mode", execution_mode.get_json()}
                    }
            );
        }

        /** @brief Simulation based on execution parameters and measure
         * parameters, where systembase parameters are loaded from
         * ``rel_systembase_params_path``. */
        static Simulation generate_simulation(
                const std::string &rel_systembase_params_path,
                EP& execution_mode,
                MS& measurement,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path, SB::name(), true))
            {
                std::cerr << "Systembase parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement", measurement.get_json()},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name() + "_path", rel_systembase_params_path},
                            {"execution_mode", execution_mode.get_json()}
                    }
            );
        }

        /** @brief Simulation based on systembase parameters and measure
         * parameters, where execution parameters are loaded from
         * ``rel_execution_mode_path``.
         */
        static Simulation generate_simulation(
                SB& systembase,
                const std::string &rel_execution_mode_path,
                MS& measurement,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path, SB::name(), true))
            {
                std::cerr << "Execution mode parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement", measurement.get_json()},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name(), systembase.get_json()},
                            {"execution_mode_path", rel_execution_mode_path}
                    }
            );
        }

        /** @brief Simulation based on measure parameters, where systembase
         * parameters are loaded from ``rel_systembase_params_path`` and
         * execution parameters from ``rel_execution_mode_path``.
         */
        static Simulation generate_simulation(
                const std::string &rel_systembase_params_path,
                const std::string &rel_execution_mode_path,
                MS& measurement,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path, SB::name(), true))
            {
                std::cerr << "Systembase parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path, SB::name(), true))
            {
                std::cerr << "Execution mode parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement", measurement.get_json()},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name() + "_path", rel_systembase_params_path},
                            {"execution_mode_path", rel_execution_mode_path}
                    }
            );
        }

        /* Constructors with measurement loaded from file */

        /** @brief Simulation based on systembase parameters and execution
         * parameters, where measure parameters are loaded from
         * ``rel_measurement_path``.
         */
        static Simulation generate_simulation(
                SB& systembase,
                EP& execution_mode,
                const std::string &rel_measurement_path,
                const std::string &running_parameter_kind="None",
                const std::string running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path, SB::name(), true))
            {
                std::cerr << "Measurement processor parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement_path", rel_measurement_path},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name(), systembase.get_json()},
                            {"execution_mode", execution_mode.get_json()}
                    }
            );
        }

        /** @brief Simulation based on execution parameters, where
         * systembase parameters are loaded from
         * ``rel_systembase_params_path`` and measure parameters from
         * ``rel_measurement_path``.
         */
        static Simulation generate_simulation(
                const std::string &rel_systembase_params_path,
                EP& execution_mode,
                const std::string &rel_measurement_path,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path, SB::name(), true))
            {
                std::cerr << "Systembase parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path, SB::name(), true))
            {
                std::cerr << "Measurement processor parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement_path", rel_measurement_path},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name() + "_path", rel_systembase_params_path},
                            {"execution_mode", execution_mode.get_json()}
                    }
            );
        }

        /** @brief Simulation based on systembase parameters, where
         * execution parameters are loaded from ``rel_execution_mode_path``
         * and measure parameters from ``rel_measurement_path``.
         */
        static Simulation generate_simulation(
                SB& systembase,
                const std::string &rel_execution_mode_path,
                const std::string &rel_measurement_path,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path, SB::name(), true))
            {
                std::cerr << "Execution mode parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path, SB::name(), true))
            {
                std::cerr << "Measurement processor parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement_path", rel_measurement_path},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name(), systembase.get_json()},
                            {"execution_mode_path", rel_execution_mode_path}
                    }
            );
        }

        /** @brief Simulation, where systembase parameters are loaded from
         * ``rel_systembase_params_path``, execution parameters from
         * ``rel_execution_mode_path`` and measure parameters from
         * ``rel_measurement_path``.
         */
        static Simulation generate_simulation(
                const std::string &rel_systembase_params_path,
                const std::string &rel_execution_mode_path,
                const std::string &rel_measurement_path,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            if(!param_helper::fs::check_if_parameter_file_exists(rel_systembase_params_path, SB::name(), true))
            {
                std::cerr << "Systembase parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if(!param_helper::fs::check_if_parameter_file_exists(rel_execution_mode_path, SB::name(), true))
            {
                std::cerr << "Execution mode parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            if(!param_helper::fs::check_if_parameter_file_exists(rel_measurement_path, SB::name(), true))
            {
                std::cerr << "Measurement processor parameters have not been found" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            return Simulation(
                    json {
                            {"measurement_path", rel_measurement_path},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name() + "_path", rel_systembase_params_path},
                            {"execution_mode_path", rel_execution_mode_path}
                    }
            );
        }

        /** @brief Prepare a simulation that will be executed from file.
         *
         * @param systembase Object of a systembase parameter class
         * @param measurement Object of a measure parameter class handling
         * gathering and writing the simulation data to file
         * @param running_parameter_kind Parent configuration module key,
         * providing the program with the necessary information of where to
         * find the running parameter. If none is given, the simulation is
         * only executed for the original set of parameters (default:
         * ``"None"``)
         * @param running_parameter Name of the running parameter (default:
         * ``"None"``)
         * @param rp_intervals List of values for the running parameter. A
         * MCMC simulation is executed for each of these values one by one
         * (default: ``std::vector<double>{0.0}``)
         */
        static Simulation prepare_simulation_from_file(
                SB& systembase,
                MS& measurement,
                const std::string &running_parameter_kind="None",
                const std::string &running_parameter="None",
                const std::vector<double>& rp_intervals=std::vector<double>{0.0}
        )
        {
            EP execution_mode;

            return Simulation(
                    json {
                            {"measurement", measurement.get_json()},
                            {"running_parameter_kind", running_parameter_kind},
                            {"running_parameter", running_parameter},
                            {"rp_intervals", rp_intervals},
                            {SB::name(), systembase.get_json()},
                            {"execution_mode", execution_mode.get_json()}
                    }
            );
        }

        /** @brief Generate a simulation solely from file.
         *
         * Uses the given paths to load an entire simulation - It is
         * important that the template parameters are in concordance with
         * the simulation parameter file.
         *
         * @param rel_sim_params_path Relative path to the
         * ``project_root_dir`` of the simulation pointing to the
         * ``sim_params.json file``
         * @param rel_execution_mode_path Relative path to the
         *  ``project_root_dir`` of the simulation pointing to the execution
         *  mode params ``.json`` file. If no path is provided, the default
         *  path is the one provided in sim_params.json.
         * @param rel_measurement_path Relative path to the
         *  ``project_root_dir`` of the simulation pointing to the
         *  measurement processing params ``.json`` file. if no path is
         *  provided, the default path is the one provided in
         *  ``sim_params.json``.
         */
        static Simulation generate_simulation_from_file(
                const std::string &rel_sim_params_path,
                const std::string &rel_execution_mode_path="",
                const std::string &rel_measurement_path="")
        {
            // Prechecks if there exists a file at rel_sim_params_path
            auto params_ = param_helper::fs::read_parameter_file(rel_sim_params_path, name());
            // Enables the possibility to run with an execution parameter or measurement parameter file that is stored somewhere else
            if(rel_execution_mode_path != "")
                params_["execution_mode_path"] = rel_execution_mode_path;
            if(rel_measurement_path != "")
                params_["measurement_path"] = rel_measurement_path;
            return Simulation<SB, EP, MS> (params_);
        }

        void initialize_simulation_parameters(const double rp=0)
        {
            if(running_parameter_ != "None") {
                markov_chain_ = execution_mode_.generate_markov_chain(running_parameter_, rp);
                update_system_params_by_new_running_parameter(rp);
            }
            else {
                markov_chain_ = execution_mode_.generate_markov_chain();
            }
        }

        /** @brief Write the simulation parameters as ``sim_params.json to
         * file``.
         *
         * @param rel_config_dir Relative path to the ``project_root_dir``
         * defined by
         * ``param_helper::proj::set_relative_path_to_project_root_dir("../")``.
         */
        void write_to_file(const std::string &rel_config_dir) {
            // Create folder in config directory if not present
            if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_config_dir))
            {
                // std::cout << "Create config directory" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_config_dir);
            }

            std::string execution_mode_path = get_entry<std::string>("execution_mode_path", rel_config_dir); // First one enables loading it from somewhere else!
            execution_mode_.write_to_file(execution_mode_path);

            std::string measurement_path = get_entry<std::string>("measurement_path", rel_config_dir); // First one enables loading it from somewhere else!
            measurement_.write_to_file(measurement_path);

            std::string systembase_params_path = get_entry<std::string>(SB::name() + "_path", rel_config_dir);
            systembase_.write_to_file(systembase_params_path);

            json execution_mode = get_entry<json>("execution_mode");
            delete_entry("execution_mode");

            json measurement = get_entry<json>("measurement");
            delete_entry("measurement");

            json systembase_params = systembase_.get_json();
            delete_entry(SB::name());

            Parameters::write_to_file(rel_config_dir, name());

            add_entry("execution_mode", execution_mode);
            add_entry("measurement", measurement);
            add_entry(SB::name(), systembase_params);

        }

        Parameters build_expanded_raw_parameters() const
        {
            Parameters parameters = Parameters::create_by_params(params_);
            parameters.add_entry(SB::name(), systembase_.build_expanded_raw_parameters().get_json());
            parameters.add_entry("execution_mode", execution_mode_.build_expanded_raw_parameters().get_json());
            parameters.add_entry("measurement", measurement_.build_expanded_raw_parameters().get_json());
            return parameters;
        }

        void update_system_params_by_new_running_parameter(double new_running_parameter) {
            auto expanded_parameters = build_expanded_raw_parameters().get_json();
            json new_system_params = update_running_parameter(
                    expanded_parameters, running_parameter_path_, new_running_parameter);
            systembase_ = SB(new_system_params[SB::name()]);
        }

        const std::vector<std::string> get_measures() const
        {
            return systembase_.get_measures();
        }

        static const std::string name()
        {
            return "sim_params";
        }
        
        /** @brief Runs the MCMC simulation and processes all measuremnts
         * based on the provided measurement processor.
         */
        void run() {
            std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

            if(running_parameter_ == "None")
                single_run();                    
            else
            {
                for(auto rp_value : rp_intervals_)
                    single_run(rp_value);
            }
        }
        
        /** @brief Executes the respective Python functions for evaluating
         *  the MCMC simulation and stores the results in
         *  ``rel_results_dir``.
         *
         *  For the evaluation to work, one needs to enable Python in CMake
         *  and initialize Python by
         *  ``mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH)`` in the
         *  main function.
         *
         *  @param rel_results_dir Relative path to the ``project_root_dir
         *  `` for storing the results
         */
        void eval(std::string rel_results_dir)
        {
            std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

            execution_mode_.evaluate(measurement_.get_rel_data_dir(), rel_results_dir, running_parameter_,
                rp_intervals_, build_expanded_raw_parameters().get_json()
            );

            // Sleep for a short moment to be able to directly use the simulation results in a subsequent Monte Carlo simulation.
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        }

        std::vector<double> rp_intervals_;
        std::string running_parameter_;

        std::string mode_;
        std::string measure_system_;
        std::string running_parameter_kind_;
        std::vector<std::string> running_parameter_path_;

    private:
        SB systembase_;
        EP execution_mode_;
        MS measurement_;

        MarkovChain markov_chain_;

        void single_run(const double rp=0) {
            initialize_simulation_parameters(rp);
            measurement_.initialize_run(mode_, running_parameter_, rp);

            write_setting_file(measurement_.get_rel_data_dir(), measurement_.get_setting_filename());
            
            markov_chain_.run<MS, SB>(measurement_, systembase_);
        }

        // To get explicit informations about the particular run
        void write_setting_file(const std::string &rel_path, const std::string &filename) const
        {
            auto raw_parameters = build_expanded_raw_parameters();
            raw_parameters.write_to_file(rel_path, filename);
        }
    };
}

#endif
