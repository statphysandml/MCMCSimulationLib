//
// Created by lukas on 31.03.20.
//

#ifndef PROGRAM_PATH_PARAMETERS_HPP
#define PROGRAM_PATH_PARAMETERS_HPP

#include <param_helper/filesystem.hpp>

namespace mcmc {
    namespace util {

        struct PathParameters {
            PathParameters(const std::string mode_type_,
                           const std::string files_dir_,
                           const std::string sim_root_dir_ = "./",
                           const bool rel_path_ = true) :
                    mode_type(mode_type_), files_dir(files_dir_), sim_root_dir(sim_root_dir_), rel_path(rel_path_) {
                written_to_file = false;
            }

            std::string get_rel_config_path() const {
                std::cout << "Create directory " << files_dir << " in configs/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + "/configs/" + files_dir, rel_path);
                return "/configs/" + files_dir + "/";
            }

            std::string get_rel_data_path() const {
                std::cout << "Create " << files_dir << " directory in data/ for config files if not existing" << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + +"/data/" + files_dir, rel_path);
                return "/data/" + files_dir + "/";
            }

            std::string get_rel_cpu_bash_script_path() const {
                std::cout << "Create " << files_dir
                          << " directory in cpu_cluster_runs/ for bash scripts and output files of respective runs"
                          << std::endl;
                param_helper::fs::generate_directory_if_not_present(sim_root_dir + +"/cpu_cluster_runs/" + files_dir);
                return "/cpu_cluster_runs/" + files_dir + "/";
            }

            const std::string mode_type;
            const std::string files_dir;
            const std::string sim_root_dir;
            const bool rel_path;
            bool written_to_file;
        };
    }
}

#endif //PROGRAM_PATH_PARAMETERS_HPP
