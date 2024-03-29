#include <mcmc/cluster_support/cpu_cluster_integration.hpp>


namespace mcmc::cluster {
    void prepare_execution_on_cpu_cluster(
        const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters,
        const std::string &executable_name, const bool run, const bool eval, const std::vector<std::string> &additional_args) {
        if (executable_name == "None") {
            std::cerr << "Executable name not properly set";
            std::exit(EXIT_FAILURE);
        }

        std::string filename = mode_type + "_" + path_parameters.target_name_;
        param_helper::fs::Fileos fileos(param_helper::proj::project_root() + "/" +
                                        path_parameters.get_rel_cpu_bash_script_dir() + "run_" + filename +
                                        ".sh");
        auto &os = fileos.get();
        os << "#!/bin/bash\n";
        os << "#PBS -l nodes=1:ppn=1:medium_bullseye\n";
        os << "#PBS -q medium_bullseye\n";
        os << "#PBS -l nice=19\n";
        os << "#PBS -l mem=2gb\n"; // 15gb
        os << "#PBS -l vmem=2gb\n"; // 15gb
        os << "#PBS -l walltime=72:00:00\n" << std::endl;
        if(strcmp(VIRTUAL_ENV, "None") != 0)
            os << "source " << mcmc::virtualenv::virtual_env_integration::g_conda_activate_path << " " << mcmc::virtualenv::virtual_env_integration::g_virtual_env << "\n" << std::endl;
        os << "cd " << param_helper::proj::project_root() << "build/" << "\n";
        os << param_helper::proj::project_root() << "build/" << executable_name
            << " "  << mode_type << " " << path_parameters.target_name_ << " " << path_parameters.sim_root_dir_
            << " "  << path_parameters.rel_path_ << " " << run << " " << eval;
        if(!additional_args.empty())
            for(const auto &additional_arg: additional_args)
                os << " " << additional_arg;
        os << std::endl;
    }

    void run_execution_on_cpu_cluster(const std::string &mode_type, const mcmc::cmdint::PathParameters &path_parameters, std::string_view cluster_mode) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::string filename = mode_type + "_" + path_parameters.target_name_;
        std::string qsubfile = param_helper::proj::project_root() + "/" +
                                path_parameters.get_rel_cpu_bash_script_dir() + "run_" + filename + ".sh";
        std::string progoutfile = param_helper::proj::project_root() + "/" +
                                    path_parameters.get_rel_cpu_bash_script_dir() + "output_run_" + filename +
                                    ".txt";
        std::string progerrfile = param_helper::proj::project_root() + "/" +
                                    path_parameters.get_rel_cpu_bash_script_dir() + "error_run_" + filename +
                                    ".txt";
        int res;
        if (cluster_mode == "on_cluster")
            res = system(("qsub -o " + progoutfile + " -e " + progerrfile + " " +
                            qsubfile).c_str()); // submit the process to queue
        else
            res = system(("nice -n 17 bash " + qsubfile + " > " + progoutfile + " 2> " +
                            progerrfile).c_str()); // execute processes on local system
        std::cout << "System code " << res << std::endl;
    }
}
