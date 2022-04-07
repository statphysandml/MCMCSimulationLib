#ifndef PROGRAM_PATH_PARAMETERS_HPP
#define PROGRAM_PATH_PARAMETERS_HPP

#include <param_helper/filesystem.hpp>

namespace mcmc {
    namespace cmdint {
        
        /** @brief Helper class for retrieving all important directories needed
         * for simulations based on arguments in the terminal or for simulations
         * on a cluster.
         */
        struct PathParameters {
            /** @brief Constructor providing the necessary information for a
             * correct storage of configuration, data and result files.
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
            PathParameters(const std::string target_name,
                           const std::string sim_root_dir="./",
                           const bool rel_path=true) :
                    target_name_(target_name), sim_root_dir_(sim_root_dir), rel_path_(rel_path),
                    rel_config_dir_(sim_root_dir + "/configs/" + target_name + "/"),
                    rel_data_dir_(sim_root_dir + "/data/" + target_name + "/"),
                    rel_results_dir_(sim_root_dir + "/results/" + target_name + "/"),
                    rel_cpu_bash_script_dir_(sim_root_dir + "/cpu_cluster_runs/" + target_name + "/"),
                    rel_gpu_bash_script_dir_(sim_root_dir + "/gpu_cluster_runs/" + target_name + "/")
                    {}

            void set_rel_config_dir(const std::string rel_config_dir)
            {
                rel_config_dir_ = rel_config_dir;
            }

            void set_rel_data_dir(const std::string rel_data_dir)
            {
                rel_data_dir_ = rel_data_dir;
            }

            void set_rel_results_dir(const std::string rel_results_dir)
            {
                rel_results_dir_ = rel_results_dir;
            }

            void set_rel_cpu_bash_script_dir(const std::string rel_cpu_bash_script_dir)
            {
                rel_cpu_bash_script_dir_ = rel_cpu_bash_script_dir;
            }

            void set_rel_gpu_bash_script_dir(const std::string rel_gpu_bash_script_dir)
            {
                rel_gpu_bash_script_dir_ = rel_gpu_bash_script_dir;
            }

            /** @brief Returns the ``rel_config_dir`` and creates the directory
             * if not present.
             */
            std::string get_rel_config_dir() const {
                // std::cout << "Create directory " << target_name_ << " in configs/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_config_dir_, rel_path_);
                return rel_config_dir_; 
            }

            /** @brief Returns the ``rel_data_dir`` and creates the directory if
             * not present.
             */
            std::string get_rel_data_dir() const {
                // std::cout << "Create " << target_name_ << " directory in data/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_data_dir_, rel_path_);
                return rel_data_dir_;
            }

            /** @brief Returns the ``rel_results_dir`` and creates the directory
             * if not present.
             */
            std::string get_rel_results_dir() const {
                // std::cout << "Create " << target_name_ << " directory in results/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_results_dir_, rel_path_);
                return rel_results_dir_;
            }

            /** @brief Returns the ``rel_cpu_bash_script_dir`` and creates the
             * directory if not present.
             */
            std::string get_rel_cpu_bash_script_dir() const {
                // std::cout << "Create " << target_name_
                //           << " directory in cpu_cluster_runs/ for bash scripts and output files of respective runs"
                //          << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_cpu_bash_script_dir_, rel_path_);
                return rel_cpu_bash_script_dir_;
            }

            /** @brief Returns the ``rel_gpu_bash_script_dir`` and creates the
             * directory if not present.
             */
            std::string get_rel_gpu_bash_script_dir() const {
                // std::cout << "Create " << target_name_
                //           << " directory in gpu_cluster_runs/ for bash scripts and output files of respective runs"
                //          << std::endl;
                param_helper::fs::generate_directory_if_not_present(rel_gpu_bash_script_dir_, rel_path_);
                return rel_gpu_bash_script_dir_;
            }

            const std::string target_name_;
            const std::string sim_root_dir_;
            const bool rel_path_;

            std::string rel_config_dir_;
            std::string rel_data_dir_;
            std::string rel_results_dir_;
            std::string rel_cpu_bash_script_dir_;
            std::string rel_gpu_bash_script_dir_;
        };
    }
}

#endif //PROGRAM_PATH_PARAMETERS_HPP
