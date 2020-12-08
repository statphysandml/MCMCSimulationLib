//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/executer.hpp"

namespace mcmc {
    namespace execution {

        std::string Executer::executable_name = "None";
        std::string Executer::cluster_mode = "locally";
        std::string Executer::conda_activate_path = "/home/lukas/.miniconda3/bin/activate";
        std::string Executer::virtual_env = "flowequation";

#ifdef PYTHON
        std::string Executer::python_scripts_path = "/home/lukas/MCMCSimulationLib/python_scripts/";

        void initialize_python() {
            Py_Initialize();
            PyRun_SimpleString(("import sys; sys.path.append('" + Executer::get_python_scripts_path() + "')").c_str());
            PyRun_SimpleString(("sys.path.append('" + Executer::get_python_scripts_path() + "/util/')").c_str());
            PyRun_SimpleString(("sys.path.append('" + Executer::get_python_scripts_path() + "/modes/')").c_str());
            /* PyRun_SimpleString("sys.path.append( os.path.dirname(os.getcwd()) + '/python_scripts/')");
            PyRun_SimpleString("sys.path.append( os.path.dirname(os.getcwd()) + '/python_scripts/plotting_environment/')");
            PyRun_SimpleString("print('Running python in ' + os.path.dirname(os.getcwd()) + '/python_scripts/')"); */
        }

        void finalize_python() {
            Py_Finalize();
        }
#endif

        const std::map<std::string, Executer::ExecutionMode> Executer::mode_resolver = {
                {"expectation_value",      expectation_value},
                {"correlation_time",       correlation_time},
                {"equilibriate",           equilibriate},
                {"plot_site_distribution", plot_site_distribution}
        };

        void initialize_executer_params(const std::string executable_name_,
                                        const std::string cluster_mode_,
                                        const std::string conda_activate_path_,
                                        const std::string virtual_env_,
                                        const std::string python_scripts_path_) {
            Executer::set_executable_name(executable_name_);
            Executer::set_cluster_mode(cluster_mode_);
            Executer::set_conda_activate_path(conda_activate_path_);
            Executer::set_virtual_env(virtual_env_);
#ifdef PYTHON
            Executer::set_python_scripts_path(python_scripts_path_);
#endif
        }

        // Preparing and executing code on the cpu cluster

        void Executer::prepare_execution_on_cpu_cluster(const std::vector<std::string> additional_args) {
            if (Executer::executable_name == "None") {
                std::cout << "Executable name not properly set";
                std::exit(EXIT_FAILURE);
            }

            std::string filename = path_parameters.mode_type + "_" + path_parameters.files_dir;
            param_helper::fs::Fileos fileos(param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "/" +
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

            os << "source " << Executer::conda_activate_path << " " << Executer::virtual_env << "\n" << std::endl;
            os << "cd " << param_helper::fs::prfs::project_root() << path_parameters.sim_root_dir << "release/" << "\n";
            // os << param_helper::fs::prfs::project_root() << path_parameters.sim_root_dir << "/" << Executer::executable_name << " " <<  path_parameters.mode_type << " " << path_parameters.files_dir << std::endl;
            os << param_helper::fs::prfs::project_root() << path_parameters.sim_root_dir << "release/" << Executer::executable_name
               << " "  << path_parameters.mode_type << " " << path_parameters.files_dir << " "
               << path_parameters.sim_root_dir << " "  << path_parameters.rel_path;
            if (additional_args.size() != 0)
                for (auto additional_arg: additional_args)
                    os << " " << additional_arg;
            os << std::endl;
        }

        void Executer::run_execution_on_cpu_cluster() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::string filename = path_parameters.mode_type + "_" + path_parameters.files_dir;
            std::string qsubfile = param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "/" +
                                   path_parameters.get_rel_cpu_bash_script_path() + "run_" + filename + ".sh";
            std::string progoutfile = param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "/" +
                                      path_parameters.get_rel_cpu_bash_script_path() + "output_run_" + filename +
                                      ".txt";
            std::string progerrfile = param_helper::fs::prfs::project_root() + path_parameters.sim_root_dir + "/" +
                                      path_parameters.get_rel_cpu_bash_script_path() + "error_run_" + filename +
                                      ".txt";
            int res;
            if (Executer::cluster_mode == "on_cluster")
                res = system(("qsub -o " + progoutfile + " -e " + progerrfile + " " +
                              qsubfile).c_str()); // submit the process to queue
            else
                res = system(("nice -n 17 bash " + qsubfile + " > " + progoutfile + " 2> " +
                              progerrfile).c_str()); // execute processes on local system
            std::cout << "System code " << res << std::endl;
        }

        // Preparing and executing code on the gpu cluster

        void Executer::prepare_execution_on_gpu_cluster() {
            std::cout << "To be implemented" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        void Executer::run_execution_on_gpu_cluster() {
            std::cout << "To be implemented" << std::endl;
            std::exit(EXIT_FAILURE);
        }

    }
}