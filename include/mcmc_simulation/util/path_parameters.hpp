//
// Created by lukas on 31.03.20.
//

#ifndef PROGRAM_PATH_PARAMETERS_HPP
#define PROGRAM_PATH_PARAMETERS_HPP

#include <param_helper/filesystem.hpp>

namespace mcmc {
    namespace util {

        struct PathParameters {
            PathParameters(const std::string files_dir_, // Directory for identifying the current simulation
                           const std::string sim_root_dir_ = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                           const bool rel_path_ = true) :
                    files_dir(files_dir_), sim_root_dir(sim_root_dir_), rel_path(rel_path_),
                    rel_config_path("/configs/" + files_dir + "/"),
                    rel_data_path("/data/" + files_dir + "/"),
                    rel_results_path("/results/" + files_dir + "/"),
                    rel_cpu_bash_script_path("/cpu_cluster_runs/" + files_dir + "/")
                    {}

            void set_rel_config_path(const std::string rel_config_path_)
            {
                rel_config_path = rel_config_path_;
            }

            void set_rel_data_path(const std::string rel_data_path_)
            {
                rel_data_path = rel_data_path_;
            }

            void set_rel_results_path(const std::string rel_results_path_)
            {
                rel_results_path = rel_results_path_;
            }

            void set_rel_cpu_bash_script_path(const std::string rel_cpu_bash_script_path_)
            {
                rel_cpu_bash_script_path = rel_cpu_bash_script_path_;
            }

            std::string get_rel_config_path() const {
                // std::cout << "Create directory " << files_dir << " in configs/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + rel_config_path, rel_path);
                return rel_config_path; 
            }

            std::string get_rel_data_path() const {
                // std::cout << "Create " << files_dir << " directory in data/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + rel_data_path, rel_path);
                return rel_data_path;
            }

            std::string get_rel_results_path() const {
                // std::cout << "Create " << files_dir << " directory in results/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + rel_results_path, rel_path);
                return rel_results_path;
            }

            std::string get_rel_cpu_bash_script_path() const {
                // std::cout << "Create " << files_dir
                //           << " directory in cpu_cluster_runs/ for bash scripts and output files of respective runs"
                //          << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + rel_cpu_bash_script_path, rel_path);
                return rel_cpu_bash_script_path;
            }

            const std::string files_dir;
            const std::string sim_root_dir;
            const bool rel_path;

            std::string rel_config_path;
            std::string rel_data_path;
            std::string rel_results_path;
            std::string rel_cpu_bash_script_path;
        };
    }
}

#endif //PROGRAM_PATH_PARAMETERS_HPP
