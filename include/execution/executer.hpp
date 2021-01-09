//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXECUTER_HPP
#define MAIN_EXECUTER_HPP

#ifdef PYTHON
#include <Python.h>
#endif

#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <thread>

#include "config.h"

#include "param_helper/params.hpp"
#include "../mcmc_simulation/util/path_parameters.hpp"
#include "../mcmc_simulation/simulation.hpp"

#include "modes/expectation_value.hpp"
#include "modes/correlation_time.hpp"
#include "modes/equilibriate.hpp"
#include "modes/plot_site_distribution.hpp"

namespace mcmc {
    namespace execution {

#ifdef PYTHON
        void initialize_python(const std::string python_modules_path_);

        void finalize_python();
#endif

        void initialize_executer_params(const std::string executable_name_,
                                        const std::string cluster_mode_);

        struct Executer {
            // Managing execution modes

            enum ExecutionMode {
                expectation_value, correlation_time, equilibriate, plot_site_distribution
            };

            static const std::map<std::string, ExecutionMode> mode_resolver;

            static std::string mode_to_string(ExecutionMode mode_) {
                switch (mode_) {
                    case expectation_value:
                        return "expectation_value";
                    case correlation_time:
                        return "correlation_time";
                    case equilibriate:
                        return "equilibriate";
                    case plot_site_distribution:
                        return "plot_site_distribution";
                    default:
                        return "mode_not_known";
                }
            }

            // Managing running modes

            enum RunningMode {
                local, on_gpu_cluster, on_cpu_cluster
            };

            // Additional static parameters for running (executer_params)
            // Project dependent variables
            static std::string python_modules_path;
            static std::string executable_name;
            static std::string cluster_mode;

            // Global parameters
            static std::string conda_activate_path;
            static std::string virtual_env;

            // Constructor
            Executer(const mcmc::util::PathParameters path_parameters_) : path_parameters(path_parameters_) {}

            /* Helper functions */

            // Preparing and executing code on the cpu cluster
            void prepare_execution_on_cpu_cluster(const std::vector<std::string> additional_args = {});
            void run_execution_on_cpu_cluster();

            // Preparing and executing code on the gpu cluster
            void prepare_execution_on_gpu_cluster();
            void run_execution_on_gpu_cluster();

            template<typename ExecutionParams, typename SBP>
            static void prep_default_execution(const mcmc::util::PathParameters path_parameters);

            template<typename SBP>
            void main() {
                ExecutionMode mode;
                // json config_file;
                bool in_preparation;

                // Load config_file from file if present
                if (param_helper::fs::check_if_parameter_file_exists(path_parameters.get_rel_config_path() + "/",
                                                                    path_parameters.mode_type + "_params",
                                                                    path_parameters.rel_path)) {
                    std::cout << "Mode = " << path_parameters.mode_type
                              << " will be executed based on provided json file"
                              << std::endl;
                    mode = mode_resolver.at(path_parameters.mode_type);
                    in_preparation = false;
                } else {
                    std::cout << "A default " << path_parameters.mode_type
                              << "_params.json file will be generated in configs/"
                              << path_parameters.files_dir
                              << "/ together with other parameter files. Adapt it to your considered set of equations and run the execution command again."
                              << std::endl;
                    mode = mode_resolver.at(path_parameters.mode_type);
                    in_preparation = true;
                }

                switch (mode) {
                    case expectation_value: {
                        if (in_preparation) {
                            prep_default_execution<ExpectationValueParameters, SBP>(path_parameters);
                        } else {
                            mcmc::simulation::SimulationParameters <SBP, ExpectationValueParameters> simparams = mcmc::simulation::SimulationParameters<SBP, ExpectationValueParameters>::generate_simulation_from_file(
                                    path_parameters.get_rel_config_path());
                            mcmc::simulation::Simulation <SBP, ExpectationValueParameters> sim(simparams);
                            sim.run();

                            #ifdef PYTHON

                            prepare_python_execution();
                            PyRun_SimpleString(("from mcmctools.modes.expectation_value import expectation_value; expectation_value('" + path_parameters.files_dir + "')").c_str());
                            finalize_python_execution();

                            /* FILE *file;
                            auto args = prepare_python_script("expectation_value");
                            PySys_SetArgv(args.first, args.second);
                            PyRun_SimpleString(("print('File', " + Executer::get_python_modules_path() + "/mcmctools/modes/expectation_value.py)").c_str());
                            file = fopen((Executer::get_python_modules_path() + "/mcmctools/modes/expectation_value.py").c_str(),"r");
                            PyRun_SimpleFile(file, "expectation_value.py");
                            fclose(file);
                            auto cwd = param_helper::fs::prfs::project_root();
                            PyRun_SimpleString(
                                    ("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str()); */

                            #endif
                        }
                        break;
                    }
                    case correlation_time: {
                        if (in_preparation) {
                            prep_default_execution<CorrelationTimeParameters, SBP>(path_parameters);
                        } else {
                            mcmc::simulation::SimulationParameters <SBP, CorrelationTimeParameters> simparams = mcmc::simulation::SimulationParameters<SBP, CorrelationTimeParameters>::generate_simulation_from_file(
                                    path_parameters.get_rel_config_path());
                            mcmc::simulation::Simulation <SBP, CorrelationTimeParameters> sim(simparams);
                            sim.run();

                            #ifdef PYTHON

                            prepare_python_execution();
                            PyRun_SimpleString(("from mcmctools.modes.correlation_time import correlation_time; correlation_time('" + path_parameters.files_dir + "')").c_str());
                            finalize_python_execution();

                            #endif
                        }
                        break;
                    }
                    case equilibriate: {
                        if (in_preparation) {
                            prep_default_execution<EquilibriateParameters, SBP>(path_parameters);
                        } else {
                            mcmc::simulation::SimulationParameters <SBP, EquilibriateParameters> simparams = mcmc::simulation::SimulationParameters<SBP, EquilibriateParameters>::generate_simulation_from_file(
                                    path_parameters.get_rel_config_path());
                            mcmc::simulation::Simulation <SBP, EquilibriateParameters> sim(simparams);
                            sim.run();

                            #ifdef PYTHON

                            prepare_python_execution();
                            PyRun_SimpleString(("from mcmctools.modes.equilibriate import equilibriate; equilibriate('" + path_parameters.files_dir + "')").c_str());
                            finalize_python_execution();

                            #endif
                        }
                        break;
                    };
                    case plot_site_distribution: {
                        if (in_preparation) {
                            prep_default_execution<PlotSiteDistributionParameters, SBP>(path_parameters);
                        } else {

                            #ifdef PYTHON

                            prepare_python_execution();
                            PyRun_SimpleString(("from mcmctools.modes.plot_site_distribution import plot_site_distribution; plot_site_distribution('" + path_parameters.files_dir + "')").c_str());
                            finalize_python_execution();

                            #endif
                        }
                        break;
                    }
                    default:
                        std::cout << "mode not known..." << std::endl;
                        break;

                        /* std::variant<ModelA, ModelB, ModelC> myModel;
                        ModelA.call(config);
                        ModelB.call(config);
                        ModelC.call(config)

                        std::visit([&](const auto& model){ model.call(); }); */

                        /* SimulationParameters<SystemBaseParams, ExpectationValueParameters> simparams(filename, dir, mode_name);
                        Simulation<SystemBaseParams, ExpectationValueParameters> sim(simparams);
                        sim.single_run(); */
                }
            }

#ifdef PYTHON
            std::pair<int, wchar_t **> prepare_python_script(std::string python_file) {
                int argc = 2;
                const char *argv[2];

                argv[0] = python_file.c_str();
                argv[1] = path_parameters.files_dir.c_str();
                PyRun_SimpleString(("import os; os.chdir('" + param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir +
                                    "')").c_str());

                // argv[2] = path_parameters.sim_root_dir.c_str(); -> go to current sim_root_dir
                /* if(path_parameters.rel_path)sdf
                    argv[3] = "True";
                else
                    argv[3] = "False"; */

                auto **_argv = (wchar_t **) PyMem_Malloc(sizeof(wchar_t *) * argc);
                for (int i = 0; i < argc; i++) {
                    wchar_t *arg = Py_DecodeLocale(argv[i], NULL);
                    _argv[i] = arg;
                }

                return std::pair<int, wchar_t **>{argc, _argv};
            }

            void prepare_python_execution() {
                PyRun_SimpleString(
                        ("import os; os.chdir('" + param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "')").c_str());
            }

            void finalize_python_execution() {
                auto cwd = param_helper::fs::prfs::project_root();
                PyRun_SimpleString(
                        ("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

            };

            static std::string get_python_modules_path() {
                return Executer::python_modules_path;
            }

            static void set_python_modules_path(const std::string python_modules_path_) {
                Executer::python_modules_path = python_modules_path_;
                std::cout << "Setting python scripts path to " << python_modules_path_ << std::endl;
            }

            static void set_conda_activate_path(const std::string conda_activate_path_) {
                Executer::conda_activate_path = conda_activate_path_;
                std::cout << "Setting conda activate path to " << Executer::conda_activate_path << std::endl;
            }

            static void set_virtual_env(const std::string virtual_env_) {
                Executer::virtual_env = virtual_env_;
                std::cout << "Setting virtual_env to " << Executer::virtual_env << std::endl;
            }

#endif

            static std::string get_executable_name() {
                return Executer::executable_name;
            }

            static void set_executable_name(const std::string executable_name_) {
                Executer::executable_name = executable_name_;
                std::cout << "Setting executable name to " << Executer::executable_name << std::endl;
            }

            static std::string get_cluster_mode() {
                return Executer::cluster_mode;
            }

            static void set_cluster_mode(const std::string cluster_mode_) {
                Executer::cluster_mode = cluster_mode_;
                std::cout << "Setting cluster mode to " << Executer::cluster_mode << std::endl;
            }

            const mcmc::util::PathParameters path_parameters;
        };

        template<typename SBP>
        void run_from_file(int argc, char **argv) {
            std::string mode_type = std::string(argv[1]);
            std::string files_dir = std::string(argv[2]);
            std::string sim_root_dir = "/./";
            bool rel_path = true;
            if (argc > 3)
                sim_root_dir = std::string(argv[3]);
            if (argc > 4) {
                std::string rel_path_boolean = std::string(argv[4]);
                if (rel_path_boolean == "false")
                    rel_path = false;
            }

            std::cout << "mode_type: " << mode_type << std::endl;
            std::cout << "files_dir: " << files_dir << std::endl;
            std::cout << "sim_root_dir: " << sim_root_dir << std::endl;
            std::cout << "rp: " << rel_path << "\n\n" << std::endl;

            mcmc::util::PathParameters path_parameters(mode_type, files_dir, sim_root_dir, rel_path);
            Executer executer(path_parameters);
            executer.main<SBP>();
        }

        /* Helper functions */

        template<typename ExecutionParams, typename SBP>
        void Executer::prep_default_execution(const mcmc::util::PathParameters path_parameters) {
            ExecutionParams expectation_value_parameters(json{});
            expectation_value_parameters.write_to_file(path_parameters.get_rel_config_path());

            mcmc::simulation::SimulationParameters <SBP, ExecutionParams> simparams = mcmc::simulation::SimulationParameters<SBP, ExecutionParams>::generate_simulation_from_file(
                    path_parameters.get_rel_config_path(),
                    path_parameters.get_rel_config_path());
        }

        template<typename SBP>
        void execute(const std::string mode_type, const std::string files_dir, const std::string sim_root_dir = "/./",
                     const bool rel_path = true, const Executer::RunningMode running_mode = Executer::local,
                     const bool execute_code = true,
                     const std::vector<std::string> additional_args = {}) {
            mcmc::util::PathParameters path_parameters(mode_type, files_dir, sim_root_dir, rel_path);
            Executer executer(path_parameters);
            if (running_mode == Executer::local and execute_code)
                // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
                executer.main<SBP>();
            else if (running_mode == Executer::on_cpu_cluster) {
                executer.prepare_execution_on_cpu_cluster(additional_args);
                if (execute_code)
                    executer.run_execution_on_cpu_cluster();
            } else if (running_mode == Executer::on_gpu_cluster) {
                executer.prepare_execution_on_gpu_cluster();
                if (execute_code)
                    executer.run_execution_on_gpu_cluster();
            }
        }
    }
}

// ./{Main} {mode_type} {files_dir} {sim_root_dir="/data/"} {rel_path=true}

#endif //MAIN_EXECUTER_HPP
