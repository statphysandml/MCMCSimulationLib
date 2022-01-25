//
// Created by lukas on 28.02.20.
//

#ifndef FROM_FILE_HPP
#define FROM_FILE_HPP


#include <iostream>
#include <map>
#include <utility>

#include <param_helper/params.hpp>

#include "mcmc_simulation/simulation.hpp"
#include "modes/mode_header.hpp"

#include "path_parameters.hpp"

#include <param_helper/json.hpp>
using json = nlohmann::json;

namespace mcmc {
    namespace cmdint {
        // Managing execution modes
        enum ExecutionMode {
            expectation_value, correlation_time, equilibrium_time, plot_site_distribution
        };

        extern const std::map<std::string, ExecutionMode> mode_resolver;

        std::string mode_to_string(ExecutionMode mode_);

        /** @brief Todo
         */
        template<typename ExecutionParams, typename SBP>
        void prep_default_execution(const mcmc::cmdint::PathParameters path_parameters) {
            ExecutionParams expectation_value_parameters(json{});
            expectation_value_parameters.write_to_file(path_parameters.get_rel_config_path());

            mcmc::simulation::SimulationParameters <SBP, ExecutionParams> simparams = mcmc::simulation::SimulationParameters<SBP, ExecutionParams>::generate_simulation_from_file(
                    path_parameters.get_rel_config_path(),
                    path_parameters.get_rel_config_path());
        }

        /** @brief Todo
         */
        template<typename SBP>
        void execute(const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters,
            const bool run = true, const bool eval = true
        )
        {
            // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
            std::setlocale(LC_ALL, "C"); // Ensures a correct reading of the number in the file name - there might be the need to adapt this in dependence on your default settings

            ExecutionMode mode;
            bool in_preparation;

            // Load config_file from file if present
            if (param_helper::fs::check_if_parameter_file_exists(path_parameters.get_rel_config_path() + "/",
                                                                 mode_type + "_params",
                                                                 path_parameters.rel_path)) {
                std::cout << " -- Mode = " << mode_type
                          << " will be executed based on provided json file -- "
                          << std::endl;
                mode = mode_resolver.at(mode_type);
                in_preparation = false;
            } else {
                std::cout << " -- A default " << mode_type
                          << "_params.json file will be generated in " << path_parameters.get_rel_config_path()
                          << "/ together with other parameter files. Adapt them and run the execution command again. --"
                          << std::endl;
                mode = mode_resolver.at(mode_type);
                in_preparation = true;
            }

            switch (mode) {
                case expectation_value: {
                    if (in_preparation) {
                        prep_default_execution<mcmc::mode::ExpectationValueParameters, SBP>(path_parameters);
                    } else {
                        mcmc::simulation::SimulationParameters <SBP, mcmc::mode::ExpectationValueParameters> simparams = mcmc::simulation::SimulationParameters<SBP, mcmc::mode::ExpectationValueParameters>::generate_simulation_from_file(
                                path_parameters.get_rel_config_path()); // ToDo: Insert mode_type params path!!
                        mcmc::simulation::Simulation <SBP, mcmc::mode::ExpectationValueParameters> sim(simparams);

                        if(run)
                            sim.run();
                        if(eval)
                            sim.eval(path_parameters.get_rel_results_path(), path_parameters.sim_root_dir);
                    }
                    break;
                }
                case correlation_time: {
                    if (in_preparation) {
                        prep_default_execution<mcmc::mode::CorrelationTimeParameters, SBP>(path_parameters);
                    } else {
                        mcmc::simulation::SimulationParameters <SBP, mcmc::mode::CorrelationTimeParameters> simparams = mcmc::simulation::SimulationParameters<SBP, mcmc::mode::CorrelationTimeParameters>::generate_simulation_from_file(
                                path_parameters.get_rel_config_path());
                        mcmc::simulation::Simulation <SBP, mcmc::mode::CorrelationTimeParameters> sim(simparams);

                        if(run)
                            sim.run();
                        if(eval)
                            sim.eval(path_parameters.get_rel_results_path(), path_parameters.sim_root_dir);
                    }
                    break;
                }
                case equilibrium_time: {
                    if (in_preparation) {
                        prep_default_execution<mcmc::mode::EquilibriumTimeParameters, SBP>(path_parameters);
                    } else {
                        mcmc::simulation::SimulationParameters <SBP, mcmc::mode::EquilibriumTimeParameters> simparams = mcmc::simulation::SimulationParameters<SBP, mcmc::mode::EquilibriumTimeParameters>::generate_simulation_from_file(
                                path_parameters.get_rel_config_path());
                        mcmc::simulation::Simulation <SBP, mcmc::mode::EquilibriumTimeParameters> sim(simparams);
                            
                        if(run)
                            sim.run();
                        if(eval)
                            sim.eval(path_parameters.get_rel_results_path(), path_parameters.sim_root_dir);
                    }
                    break;
                };
                default:
                    std::cerr << "mode not known..." << std::endl;
                    break;

                /* std::variant<ModelA, ModelB, ModelC> myModel;
                    ModelA.call(config);
                    ModelB.call(config);
                    ModelC.call(config)

                    std::visit([&](const auto& model){ model.call(); }); */
            }
        }

        template<typename SBP>
        struct CmdIntSim
        {
            CmdIntSim(const std::string target_name, // 
                      const std::string sim_root_dir = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                      const bool rel_path = true) : path_parameters(mcmc::cmdint::PathParameters(target_name, sim_root_dir, rel_path))
            {}

            void main(int argc, char **argv)
            {
                if(argc > 1)
                {
                    execute(argc, argv);
                }
                else
                {
                    // Helpful for a preparation of the simulation or immediate execution (on cpu/gpu/locally, testing/running directly)
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
                    if (rel_path_boolean == "false")
                        rel_path = false;
                }
                if (argc > 5) {
                    std::string run_boolean = std::string(argv[5]);
                    if (run_boolean == "false")
                        run = false;
                }
                if (argc > 6) {
                    std::string eval_boolean = std::string(argv[6]);
                    if (eval_boolean == "false")
                        eval = false;
                }

                /* std::cout << "mode_type: " << mode_type << std::endl;
                std::cout << "target_name: " << target_name << std::endl;
                std::cout << "sim_root_dir: " << sim_root_dir << std::endl;
                std::cout << "rp: " << rel_path << "\n\n" << std::endl; */

                mcmc::cmdint::PathParameters path_parameters_(target_name, sim_root_dir, rel_path);
                mcmc::cmdint::execute<SBP>(mode_type, path_parameters_, run, eval);
            }

            virtual void prepare() = 0;

            mcmc::cmdint::PathParameters path_parameters;
        };
    }
}

// ./{Main} {mode_type} {target_name} {sim_root_dir="/data/"} {rel_path=true}

#endif //FROM_FILE_HPP
