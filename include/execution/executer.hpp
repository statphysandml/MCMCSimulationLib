//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXECUTER_HPP
#define MAIN_EXECUTER_HPP

#ifdef RUN_WITH_PYTHON_BACKEND
#include <pybind11/embed.h>
namespace py = pybind11;
#endif

#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <thread>

#include "config.h"

#include <param_helper/params.hpp>

#include "../mcmc_simulation/util/path_parameters.hpp"
#include "../mcmc_simulation/simulation.hpp"

#include "modes/expectation_value.hpp"
#include "modes/correlation_time.hpp"
#include "modes/equilibrium_time.hpp"

#include <param_helper/json.hpp>
using json = nlohmann::json;

namespace mcmc {
    namespace execution {

#ifdef RUN_WITH_PYTHON_BACKEND
        void initialize_python(const std::string python_modules_path_);
        void finalize_python();
#endif

        void initialize_executer_params(const std::string executable_name_,
                                        const std::string cluster_mode_);

        struct Executer {
            // Managing execution modes

            enum ExecutionMode {
                expectation_value, correlation_time, equilibrium_time, plot_site_distribution
            };

            static const std::map<std::string, ExecutionMode> mode_resolver;

            static std::string mode_to_string(ExecutionMode mode_) {
                switch (mode_) {
                    case expectation_value:
                        return "expectation_value";
                    case correlation_time:
                        return "correlation_time";
                    case equilibrium_time:
                        return "equilibrium_time";
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
            void prepare_execution_on_cpu_cluster(const std::string mode_type, const std::vector<std::string> additional_args = {});
            void run_execution_on_cpu_cluster(const std::string mode_type);

            // Preparing and executing code on the gpu cluster
            void prepare_execution_on_gpu_cluster(const std::string mode_type);
            void run_execution_on_gpu_cluster(const std::string mode_type);

            template<typename ExecutionParams, typename SBP>
            static void prep_default_execution(const mcmc::util::PathParameters path_parameters);

            template<typename SBP>
            void main(const std::string mode_type) {
                ExecutionMode mode;
                // json config_file;
                bool in_preparation;

                // Load config_file from file if present
                if (param_helper::fs::check_if_parameter_file_exists(path_parameters.get_rel_config_path() + "/",
                                                                     mode_type + "_params",
                                                                     path_parameters.rel_path)) {
                    std::cout << "Mode = " << mode_type
                              << " will be executed based on provided json file"
                              << std::endl;
                    mode = mode_resolver.at(mode_type);
                    in_preparation = false;
                } else {
                    std::cout << "A default " << mode_type
                              << "_params.json file will be generated in " << path_parameters.get_rel_config_path()
                              << "/ together with other parameter files. Adapt it to your considered set of equations and run the execution command again."
                              << std::endl;
                    mode = mode_resolver.at(mode_type);
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

                            uint number_of_measurements = simparams.execution_params->get_json()["number_of_measurements"].template get<uint>();
                            uint n_means_bootstrap = simparams.execution_params->get_json()["n_means_bootstrap"].template get<uint>();
                            const json& post_measures = simparams.execution_params->get_json()["post_measures"].template get<json>();
                            const json& measures = simparams.get_measures();

                            #ifdef RUN_WITH_PYTHON_BACKEND
                            py::exec("from mcmctools.modes.expectation_value import expectation_value");
                            py::exec("from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func");
                            py::exec("from mcmctools.utils.utils import retrieve_rp_keys");
                            py::exec(("expectation_value(\
                                measures=" + param_helper::params::merge_list_like<std::string>(measures, post_measures).dump() + ",\
                                running_parameter='" + simparams.running_parameter + "',\
                                rp_keys=retrieve_rp_keys(running_parameter='" + simparams.running_parameter + "',\
                                    rp_minimum=" + std::to_string(simparams.rp_minimum) + ",\
                                    rp_maximum=" + std::to_string(simparams.rp_maximum) + ",\
                                    rp_number=" + std::to_string(simparams.rp_number) + "),\
                                rel_data_dir='" + path_parameters.get_rel_data_path() + "',\
                                number_of_measurements=" + std::to_string(number_of_measurements) + ",\
                                n_means_bootstrap=" + std::to_string(n_means_bootstrap) + ",\
                                rel_results_dir='" + path_parameters.get_rel_results_path() + "',\
                                project_base_dir='" + param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "',\
                                custom_measures_func=get_custom_measures_func(), custom_measures_args='" + simparams.build_expanded_raw_parameters().get_json().dump() + "',\
                                custom_load_data_func=get_custom_load_data_func(), custom_load_data_args='" + simparams.build_expanded_raw_parameters().get_json().dump() + "')").c_str());
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

                            uint minimum_sample_size = simparams.execution_params->get_json()["minimum_sample_size"].template get<uint>();
                            uint maximum_correlation_time = simparams.execution_params->get_json()["maximum_correlation_time"].template get<uint>();
                            std::string measure = simparams.execution_params->get_json()["measure"].template get<std::string>();

                            #ifdef RUN_WITH_PYTHON_BACKEND
                            py::exec("from mcmctools.modes.correlation_time import correlation_time");
                            py::exec("from mcmctools.utils.utils import retrieve_rp_keys");
                            py::exec(("correlation_time(\
                                minimum_sample_size=" + std::to_string(minimum_sample_size) + ",\
                                maximum_correlation_time=" + std::to_string(maximum_correlation_time) + ",\
                                measure='" + measure + "',\
                                running_parameter='" + simparams.running_parameter + "',\
                                rp_keys=retrieve_rp_keys(running_parameter='" + simparams.running_parameter + "',\
                                    rp_minimum=" + std::to_string(simparams.rp_minimum) + ",\
                                    rp_maximum=" + std::to_string(simparams.rp_maximum) + ",\
                                    rp_number=" + std::to_string(simparams.rp_number) + "),\
                                rel_data_dir='" + path_parameters.get_rel_data_path() + "',\
                                rel_results_dir='" + path_parameters.get_rel_results_path() + "',\
                                project_base_dir='" + param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "',\
                                fma=fma)").c_str());
                            #endif
                        }
                        break;
                    }
                    case equilibrium_time: {
                        if (in_preparation) {
                            prep_default_execution<EquilibriumTimeParameters, SBP>(path_parameters);
                        } else {
                            mcmc::simulation::SimulationParameters <SBP, EquilibriumTimeParameters> simparams = mcmc::simulation::SimulationParameters<SBP, EquilibriumTimeParameters>::generate_simulation_from_file(
                                    path_parameters.get_rel_config_path());
                            mcmc::simulation::Simulation <SBP, EquilibriumTimeParameters> sim(simparams);
                            sim.run();

                            uint sample_size = simparams.execution_params->get_json()["sample_size"].template get<uint>();
                            uint number_of_steps = simparams.execution_params->get_json()["number_of_steps"].template get<uint>();
                            std::string measure = simparams.execution_params->get_json()["measure"].template get<std::string>();
                            double confidence_range = simparams.execution_params->get_json()["confidence_range"].template get<double>();
                            uint confidence_window = simparams.execution_params->get_json()["confidence_window"].template get<uint>();

                            #ifdef RUN_WITH_PYTHON_BACKEND
                            py::exec("from mcmctools.modes.equilibrium_time import equilibrium_time");
                            py::exec("from mcmctools.utils.utils import retrieve_rp_keys");
                            py::exec(("equilibrium_time(\
                                sample_size=" + std::to_string(sample_size) + ",\
                                number_of_steps=" + std::to_string(number_of_steps) + ",\
                                measure='" + measure + "',\
                                confidence_range=" + std::to_string(confidence_range) + ",\
                                confidence_window=" + std::to_string(confidence_window) + ",\
                                running_parameter='" + simparams.running_parameter + "',\
                                rp_keys=retrieve_rp_keys(running_parameter='" + simparams.running_parameter + "',\
                                    rp_minimum=" + std::to_string(simparams.rp_minimum) + ",\
                                    rp_maximum=" + std::to_string(simparams.rp_maximum) + ",\
                                    rp_number=" + std::to_string(simparams.rp_number) + "),\
                                rel_data_dir='" + path_parameters.get_rel_data_path() + "',\
                                rel_results_dir='" + path_parameters.get_rel_results_path() + "',\
                                project_base_dir='" + param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "',\
                                fma=fma)").c_str());
                            #endif
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

                        /* SimulationParameters<SystemBaseParams, ExpectationValueParameters> simparams(filename, dir, mode_name);
                        Simulation<SystemBaseParams, ExpectationValueParameters> sim(simparams);
                        sim.single_run(); */
                }
            }

#ifdef RUN_WITH_PYTHON_BACKEND
            static std::string get_python_modules_path() {
                return Executer::python_modules_path;
            }

            static void set_python_modules_path(const std::string python_modules_path_) {
                Executer::python_modules_path = python_modules_path_;
                std::cout << "Setting python scripts path to " << python_modules_path_ << std::endl;
            }
#endif
#ifdef RUN_WITH_VIRTUAL_ENV
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
            std::string sim_root_dir = "./";
            bool rel_path = true;
            if (argc > 3)
                sim_root_dir = std::string(argv[3]);
            if (argc > 4) {
                std::string rel_path_boolean = std::string(argv[4]);
                if (rel_path_boolean == "false")
                    rel_path = false;
            }

            /* std::cout << "mode_type: " << mode_type << std::endl;
            std::cout << "files_dir: " << files_dir << std::endl;
            std::cout << "sim_root_dir: " << sim_root_dir << std::endl;
            std::cout << "rp: " << rel_path << "\n\n" << std::endl; */

            mcmc::util::PathParameters path_parameters(files_dir, sim_root_dir, rel_path);
            Executer executer(path_parameters);
            executer.main<SBP>(mode_type);
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
        void execute(const std::string mode_type, const std::string files_dir, const std::string sim_root_dir = "./",
                     const bool rel_path = true, const Executer::RunningMode running_mode = Executer::local,
                     const bool execute_code = true,
                     const std::vector<std::string> additional_args = {}, const std::string rel_config_path="",
                     const std::string rel_data_path="", const std::string rel_results_path="",
                     const std::string rel_cpu_bash_script_path="") {
            mcmc::util::PathParameters path_parameters(files_dir, sim_root_dir, rel_path);
            if(rel_config_path != "")
                path_parameters.set_rel_config_path(rel_config_path);
            if(rel_data_path != "")
                path_parameters.set_rel_data_path(rel_data_path);
            if(rel_results_path != "")
                path_parameters.set_rel_results_path(rel_results_path);
            if(rel_cpu_bash_script_path != "")
                path_parameters.set_rel_cpu_bash_script_path(rel_cpu_bash_script_path);

            Executer executer(path_parameters);
            if (running_mode == Executer::local and execute_code)
                // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
                executer.main<SBP>(mode_type);
            else if (running_mode == Executer::on_cpu_cluster) {
                executer.prepare_execution_on_cpu_cluster(mode_type, additional_args);
                if (execute_code)
                    executer.run_execution_on_cpu_cluster(mode_type);
            } else if (running_mode == Executer::on_gpu_cluster) {
                executer.prepare_execution_on_gpu_cluster(mode_type);
                if (execute_code)
                    executer.run_execution_on_gpu_cluster(mode_type);
            }
        }
    }
}

// ./{Main} {mode_type} {files_dir} {sim_root_dir="/data/"} {rel_path=true}

#endif //MAIN_EXECUTER_HPP
