//
// Created by lukas on 31.03.20.
//

#ifndef PROGRAM_PATH_PARAMETERS_HPP
#define PROGRAM_PATH_PARAMETERS_HPP

#include <boost/filesystem.hpp>

struct PathParameters
{
    PathParameters(const std::string mode_type_,
                   const std::string files_dir_,
                   const std::string sim_root_dir_="/./",
                   const bool rel_path_=true) :
            mode_type(mode_type_), files_dir(files_dir_), sim_root_dir(sim_root_dir_), rel_path(rel_path_)
    {
        written_to_file = false;
    }

    std::string get_rel_config_path() const
    {
        if(!boost::filesystem::is_directory(gcp() + sim_root_dir + "/configs/" + files_dir)) {
            std::cout << "Create " << files_dir << " directory in configs/ for config files" << std::endl;
            boost::filesystem::create_directories(gcp() + sim_root_dir + "/configs/" + files_dir);
        }
        return "/configs/" + files_dir + "/";
    }

    std::string get_rel_data_path() const
    {
        if(!boost::filesystem::is_directory(gcp() + sim_root_dir +  + "/data/" + files_dir)) {
            std::cout << "Create " << files_dir << " directory in data/ for config files" << std::endl;
            boost::filesystem::create_directories(gcp() + sim_root_dir +  "/data/" + files_dir);
        }
        return "/data/" + files_dir + "/";
    }

    std::string get_rel_cpu_bash_script_path() const
    {
        if(!boost::filesystem::is_directory(gcp() + sim_root_dir +  + "/cpu_cluster_runs/" + files_dir)) {
            std::cout << "Create " << files_dir << " directory in cpu_cluster_runs/ for bash scripts and output files of respective runs" << std::endl;
            boost::filesystem::create_directories(gcp() + sim_root_dir +  "/cpu_cluster_runs/" + files_dir);
        }
        return "/cpu_cluster_runs/" + files_dir + "/";
    }

    const std::string mode_type;
    const std::string files_dir;
    const std::string sim_root_dir; // ToDo: Currently not used! -> might be set per default on gcp() and optionally on some other dir where a configs and a data directory are generated
    const bool rel_path;
    bool written_to_file;
};

#endif //PROGRAM_PATH_PARAMETERS_HPP
