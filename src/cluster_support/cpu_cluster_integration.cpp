#include "../../include/cluster_support/cpu_cluster_integration.hpp"


namespace mcmc {
    namespace cluster {
        void prepare_execution_on_cpu_cluster(
            const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters,
            const std::string executable_name, const bool eval, const bool run, const std::vector<std::string> additional_args) {
            if (executable_name == "None") {
                std::cerr << "Executable name not properly set";
                std::exit(EXIT_FAILURE);
            }

            std::string filename = mode_type + "_" + path_parameters.target_name;
            param_helper::fs::Fileos fileos(param_helper::proj::project_root() + path_parameters.sim_root_dir + "/" +
                                           path_parameters.get_rel_cpu_bash_script_path() + "run_" + filename +
                                           ".sh");
            auto &os = fileos.get();
            os << "#!/bin/bash\n";
            os << "#PBS -l nodes=1:ppn=1:medium_buster\n";
            os << "#PBS -q medium_buster\n";
            os << "#PBS -l nice=19\n";
            os << "#PBS -l mem=2gb\n"; // 15gb
            os << "#PBS -l vmem=2gb\n"; // 15gb
            os << "#PBS -l walltime=72:00:00\n" << std::endl;

#ifdef RUN_WITH_VITUAL_ENV
            os << "source " << mcmc::virtualenv::conda_activate_path << " " << mcmc::virtualenv::virtual_env << "\n" << std::endl;
#endif
            os << "cd " << param_helper::proj::project_root() << path_parameters.sim_root_dir << "build/" << "\n";
            // os << param_helper::proj::project_root() << path_parameters.sim_root_dir << "/" << Executer::executable_name << " " <<  mode_type << " " << path_parameters.target_name << std::endl;
            os << param_helper::proj::project_root() << path_parameters.sim_root_dir << "build/" << executable_name
               << " "  << mode_type << " " << path_parameters.target_name << " "
               << path_parameters.sim_root_dir << " "  << path_parameters.rel_path << " " << run << " " << eval;
            if (additional_args.size() != 0)
                for (auto additional_arg: additional_args)
                    os << " " << additional_arg;
            os << std::endl;
        }

        void run_execution_on_cpu_cluster(const std::string mode_type, const mcmc::cmdint::PathParameters path_parameters, const std::string cluster_mode) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::string filename = mode_type + "_" + path_parameters.target_name;
            std::string qsubfile = param_helper::proj::project_root() + path_parameters.sim_root_dir + "/" +
                                   path_parameters.get_rel_cpu_bash_script_path() + "run_" + filename + ".sh";
            std::string progoutfile = param_helper::proj::project_root() + path_parameters.sim_root_dir + "/" +
                                      path_parameters.get_rel_cpu_bash_script_path() + "output_run_" + filename +
                                      ".txt";
            std::string progerrfile = param_helper::proj::project_root() + path_parameters.sim_root_dir + "/" +
                                      path_parameters.get_rel_cpu_bash_script_path() + "error_run_" + filename +
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
}