#ifndef PROGRAM_PATH_PARAMETERS_HPP
#define PROGRAM_PATH_PARAMETERS_HPP

#include <param_helper/filesystem.hpp>

namespace mcmc {
    namespace cmdint {
        
        /** @brief Helper class for retrieving all important directories needed for simulations
         * based on arguments in the terminal or for simulations on a cluster
         *
         */
        struct PathParameters {
            /** @brief Constructor providing the necessary information for a correct storage of config and results files
             *
             * @param target_name_ Identifier of the current simulation (the directory is used for storing files in
             * sim_root_dir/config/, sim_root_dir/data/, sim_root_dir/results/, sim_root_dir/cpu_cluster_runs/, sim_root_dir/gpu_cluster_runs/ throughout the simulation)
             * @param sim_root_dir_ Relative (to the project_root_dir set by param_helper::proj::set_relative_path_to_project_root_dir("../")) or absolute path to the simulation directory used for storing the simulation config and results files
             * @param rel_path_ Whether sim_root_dir is the relative (true) or the absolute path (false) to the project_root_dir
             */
            PathParameters(const std::string target_name_, // 
                           const std::string sim_root_dir_ = "./", // Relative path from project_root to simulation_root or absolute path to simulation root
                           const bool rel_path_ = true) :
                    target_name(target_name_), sim_root_dir(sim_root_dir_), rel_path(rel_path_),
                    rel_config_path(sim_root_dir_ + "/configs/" + target_name + "/"),
                    rel_data_path(sim_root_dir_ + "/data/" + target_name + "/"),
                    rel_results_path(sim_root_dir_ + "/results/" + target_name + "/"),
                    rel_cpu_bash_script_path(sim_root_dir_ + "/cpu_cluster_runs/" + target_name + "/"),
                    rel_gpu_bash_script_path(sim_root_dir_ + "/gpu_cluster_runs/" + target_name + "/")
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

            void set_rel_gpu_bash_script_path(const std::string rel_gpu_bash_script_path_)
            {
                rel_gpu_bash_script_path = rel_gpu_bash_script_path_;
            }

            /** @brief Returns the rel_config_path and creates the directory if not present
             */
            std::string get_rel_config_path() const {
                // std::cout << "Create directory " << target_name << " in configs/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_config_path, rel_path);
                return rel_config_path; 
            }

            /** @brief Returns the rel_data_path and creates the directory if not present
             */
            std::string get_rel_data_path() const {
                // std::cout << "Create " << target_name << " directory in data/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_data_path, rel_path);
                return rel_data_path;
            }

            /** @brief Returns the rel_results_path and creates the directory if not present
             */
            std::string get_rel_results_path() const {
                // std::cout << "Create " << target_name << " directory in results/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_results_path, rel_path);
                return rel_results_path;
            }

            /** @brief Returns the rel_cpu_bash_script_path and creates the directory if not present
             */
            std::string get_rel_cpu_bash_script_path() const {
                // std::cout << "Create " << target_name
                //           << " directory in cpu_cluster_runs/ for bash scripts and output files of respective runs"
                //          << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_cpu_bash_script_path, rel_path);
                return rel_cpu_bash_script_path;
            }

            /** @brief Returns the rel_gpu_bash_script_path and creates the directory if not present
             */
            std::string get_rel_gpu_bash_script_path() const {
                // std::cout << "Create " << target_name
                //           << " directory in gpu_cluster_runs/ for bash scripts and output files of respective runs"
                //          << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_gpu_bash_script_path, rel_path);
                return rel_gpu_bash_script_path;
            }

            const std::string target_name;
            const std::string sim_root_dir;
            const bool rel_path;

            std::string rel_config_path;
            std::string rel_data_path;
            std::string rel_results_path;
            std::string rel_cpu_bash_script_path;
            std::string rel_gpu_bash_script_path;
        };
    }
}

#endif //PROGRAM_PATH_PARAMETERS_HPP
