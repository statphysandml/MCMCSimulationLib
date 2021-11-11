//
// Created by lukas on 28.02.20.
//
#include "../../include/execution/executer.hpp"

namespace mcmc {
    namespace execution {

        // Project dependent parameters
        std::string Executer::executable_name = "None";
        std::string Executer::cluster_mode = "local";

#ifdef RUN_WITH_VITUAL_ENV
        // Global parameters
        std::string Executer::conda_activate_path = CONDA_ACTIVATE_PATH;
        std::string Executer::virtual_env = VIRTUAL_ENV;
#endif

#ifdef PYTHON
        // Project dependent parameter
        std::string Executer::python_modules_path = "";

        void initialize_python(const std::string python_modules_path_) {
            Py_Initialize();
            if(python_modules_path_ != "")
            {
                Executer::set_python_modules_path(python_modules_path_);
                PyRun_SimpleString(("import sys; sys.path.append('" + Executer::get_python_modules_path() + "')").c_str());
            }
            PyRun_SimpleString("import sys; print('Python system path:',  sys.path)");
        }

        void finalize_python() {
            Py_Finalize();
        }
#endif

        const std::map<std::string, Executer::ExecutionMode> Executer::mode_resolver = {
                {"expectation_value",      expectation_value},
                {"correlation_time",       correlation_time},
                {"equilibriate",           equilibriate}
        };

        void initialize_executer_params(const std::string executable_name_,
                                        const std::string cluster_mode_) {
            Executer::set_executable_name(executable_name_);
            Executer::set_cluster_mode(cluster_mode_);
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

#ifdef RUN_WITH_VITUAL_ENV
            os << "source " << Executer::conda_activate_path << " " << Executer::virtual_env << "\n" << std::endl;
#endif
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
