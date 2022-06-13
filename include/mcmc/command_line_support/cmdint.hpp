#ifndef FROM_FILE_HPP
#define FROM_FILE_HPP


#include <iostream>
#include <map>
#include <utility>

#include <param_helper/params.hpp>

#include <mcmc/mcmc_simulation/simulation.hpp>
#include <mcmc/modes/mode_header.hpp>

#include <mcmc/command_line_support/path_parameters.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace mcmc::cmdint {
    // Managing execution modes
    enum ExecutionMode {
        expectation_value, correlation_time, equilibrium_time, plot_site_distribution
    };

    extern const std::map<std::string, ExecutionMode> g_mode_resolver;

    std::string mode_to_string(ExecutionMode mode_);

    /** @brief Helper function for preparing a simulation based on the
     * default arguments of the respective template class arguments.
     *
     * @param path_parameters Path parameters containing all important paths
     * for finding the necessary configurations files and for storing data
     * and results.
     */
    template<typename ExecutionParams, typename SB, typename MS>
    void prep_default_execution(const mcmc::cmdint::PathParameters path_parameters) {
        SB systembase;
        MS measurement_processor(path_parameters.get_rel_data_dir());
        auto simulation = mcmc::simulation::Simulation<SB, ExecutionParams, MS>::prepare_simulation_from_file(
            systembase,
            measurement_processor
        );
        simulation.write_to_file(path_parameters.get_rel_config_dir());
    }

    /** @brief Helper function for preparing and executing simulations based
     * on a mode type and path parameters.
     *
     * @param mode_type The execution mode used for the simulation
     * (``"equilibrium_time"``, ``"correlation_time"`` or
     * ``"expectation_value"``)
     * @param path_paramers Path parameters containing all important paths
     * for finding the necessary configuration files and for storing data
     * and results
     * @param run Indicate whether the MCMC simulation is performed
     * (``true``) or not (``false``)
     * @param eval Indicate whether the evaluation should be performed in
     * the same run (``true``) or not (``false``)
     */
    template<typename SB, typename MS>
    void execute(const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters,
        const bool run = true, const bool eval = true
    )
    {
        std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

        ExecutionMode mode;
        bool in_preparation;

        // Load config_file from file if present
        if (param_helper::fs::check_if_parameter_file_exists(path_parameters.get_rel_config_dir() + "/",
                                                                mode_type + "_params",
                                                                path_parameters.rel_path_)) {
            std::cout << " -- Mode = " << mode_type
                        << " will be executed based on the provided json files -- "
                        << std::endl;
            mode = g_mode_resolver.at(mode_type);
            in_preparation = false;
        } else {
            std::cout << " -- A default " << mode_type
                        << "_params.json file is generated in " << path_parameters.get_rel_config_dir()
                        << "/ together with other parameter files. Adapt them and run the execution command again. --"
                        << std::endl;
            mode = g_mode_resolver.at(mode_type);
            in_preparation = true;
        }

        switch (mode) {
            case expectation_value: {
                if (in_preparation) {
                    prep_default_execution<mcmc::mode::ExpectationValue, SB, MS>(path_parameters);
                } else {
                    auto simulation = mcmc::simulation::Simulation<SB, mcmc::mode::ExpectationValue, MS>::generate_simulation_from_file(
                        path_parameters.get_rel_config_dir()
                    );
                    
                    if(run)
                        simulation.run();
                    if(eval)
                        simulation.eval(path_parameters.get_rel_results_dir());
                }
                break;
            }
            case correlation_time: {
                if (in_preparation) {
                    prep_default_execution<mcmc::mode::CorrelationTime, SB, MS>(path_parameters);
                } else {
                    auto simulation = mcmc::simulation::Simulation<SB, mcmc::mode::CorrelationTime, MS>::generate_simulation_from_file(
                        path_parameters.get_rel_config_dir()
                    );

                    if(run)
                        simulation.run();
                    if(eval)
                        simulation.eval(path_parameters.get_rel_results_dir());
                }
                break;
            }
            case equilibrium_time: {
                if (in_preparation) {
                    prep_default_execution<mcmc::mode::EquilibriumTime, SB, MS>(path_parameters);
                } else {
                    auto simulation = mcmc::simulation::Simulation<SB, mcmc::mode::EquilibriumTime, MS>::generate_simulation_from_file(
                        path_parameters.get_rel_config_dir()
                    );
                        
                    if(run)
                        simulation.run();
                    if(eval)
                        simulation.eval(path_parameters.get_rel_results_dir());
                }
                break;
            };
            default:
                std::cerr << "execution mode not known..." << std::endl;
                break;
        }
    }

    /** @brief Base strcut for preparing and executing a simulation from the
     * command line.
     */
    template<typename SB, typename MS>
    struct CmdIntSim
    {
        /** @brief Constructor for providing all important path variables.
         * 
         * @param target_name Identifier of the current simulation (the
         * directory is used for storing files in ``sim_root_dir/config/``,
         * ``sim_root_dir/data/``, ``sim_root_dir/results/``,
         * ``sim_root_dir/cpu_cluster_runs/``,
         * ``sim_root_dir/gpu_cluster_runs/`` throughout the simulation)
         * @param sim_root_dir Relative path to the ``project_root_dir``
         * (set by
         * ``param_helper::proj::set_relative_path_to_project_root_dir("../")``)
         * or absolute path to the simulation directory used for storing the
         * configuration, data and result files of the simulation 
         * @param rel_path Variable indicating whether ``sim_root_dir``
         * refers to an absolute (``false``) or a relative (``true``) path
         */
        CmdIntSim(const std::string &target_name, // 
                    const std::string &sim_root_dir="./",
                    const bool rel_path=true) : path_parameters_(mcmc::cmdint::PathParameters(target_name, sim_root_dir, rel_path))
        {}

        /** @brief Main function for executing or preparing the simulation
         * from file. If ``argc=0``, the overloaded ``prepare`` function is
         * called, otherwise the simulation is executed with the help of the
         * ``mcmc::cmdint::execute`` function.
         */
        void main(int argc, char **argv)
        {
            if(argc > 1)
            {
                execute(argc, argv);
            }
            else
            {
                prepare();
            }
        }

        void execute(int argc, char **argv) {
            std::string mode_type = std::string(argv[1]);
            std::string target_name = std::string(argv[2]);
            std::string sim_root_dir = "./";
            bool rel_path = true;
            bool run = true;
            bool eval = true;
            if (argc > 3)
                sim_root_dir = std::string(argv[3]);
            if (argc > 4) {
                std::string rel_path_boolean = std::string(argv[4]);
                if (rel_path_boolean == "false" or rel_path_boolean == "0")
                    rel_path = false;
            }
            if (argc > 5) {
                std::string run_boolean = std::string(argv[5]);
                if (run_boolean == "false" or run_boolean == "0")
                    run = false;
            }
            if (argc > 6) {
                std::string eval_boolean = std::string(argv[6]);
                if (eval_boolean == "false" or eval_boolean == "0")
                    eval = false;
            }

            /* std::cout << "mode_type: " << mode_type << std::endl;
            std::cout << "target_name: " << target_name << std::endl;
            std::cout << "sim_root_dir: " << sim_root_dir << std::endl;
            std::cout << "rp: " << rel_path << "\n\n" << std::endl; */

            mcmc::cmdint::PathParameters path_parameters(target_name, sim_root_dir, rel_path);
            mcmc::cmdint::execute<SB, MS>(mode_type, path_parameters, run, eval);
        }

        /** @brief Virtual method which is supposed to be overloaded.
         *
         * Helpful for a preparation of the simulation or an immediate
         * execution (on cpu/gpu/locally, testing/running directly)
         */
        virtual void prepare()
        {}

        mcmc::cmdint::PathParameters path_parameters_;
    };
}

// ./{executable} {execution_mode} {target_name} {sim_root_dir="./"} {rel_path=true} {run=true} {eval=true}

#endif //FROM_FILE_HPP
