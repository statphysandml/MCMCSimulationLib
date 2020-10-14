//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXECUTER_HPP
#define MAIN_EXECUTER_HPP

#include <Python.h>

#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <thread>

#include "param_helper/params.hpp"
#include "../mcmc_simulation/util/path_parameters.hpp"
#include "../mcmc_simulation/simulation.hpp"

#include "modes/expectation_value.hpp"
#include "modes/correlation_time.hpp"
#include "modes/equilibriate.hpp"
#include "modes/plot_site_distribution.hpp"


void initialize_python();
void finalize_python();

void initialize_executer_params(const std::string executable_name_, const std::string python_scripts_path_, const std::string cluster_mode_,
                                const std::string conda_activate_path_, const std::string virtual_env_);

struct Executer
{
    static std::string python_scripts_path;
    static std::string executable_name;
    static std::string cluster_mode;
    static std::string conda_activate_path;
    static std::string virtual_env;

    enum ExecutionMode {expectation_value, correlation_time, equilibriate, plot_site_distribution};
    static const std::map< std::string, ExecutionMode> mode_resolver;

    enum RunningMode {local, on_gpu_cluster, on_cpu_cluster};

    Executer(const PathParameters path_parameters_) : path_parameters(path_parameters_)
    {}

    static std::string mode_to_string (ExecutionMode mode_) {
        switch (mode_) {
            case expectation_value:
                return "expectation_value";
            case correlation_time:
                return "correlation_time";
            case equilibriate:
                return "equilibriate";
            case plot_site_distribution:
                return "plot_site_distribution";
            default:
                return "mode_not_known";
        }
    }

    template<typename ExecutionParams, typename SBP>
    static void prep_default_execution(const PathParameters path_parameters);

    // Helper functions!!

    void prepare_execution_on_cpu_cluster(const std::vector<std::string> additional_args={}) {
        if(Executer::executable_name == "None")
        {
            std::cout << "Executable name not properly set";
            std::exit(EXIT_FAILURE);
        }

        std::string filename = path_parameters.mode_type + "_" + path_parameters.files_dir;
        Fileos fileos (gcp() + path_parameters.sim_root_dir + "/" + path_parameters.get_rel_cpu_bash_script_path() + "run_" + filename + ".sh");
        auto& os = fileos.get();
        os << "#!/bin/bash\n";
        os << "#PBS -l nodes=1:ppn=1:medium_buster\n";
        os << "#PBS -q medium_buster\n";
        os << "#PBS -l nice=19\n";
        os << "#PBS -l mem=2gb\n";
        os << "#PBS -l vmem=2gb\n";
        os << "#PBS -l walltime=72:00:00\n" << std::endl;

        os << "source " << Executer::conda_activate_path << " " << Executer::virtual_env << "\n" << std::endl;
        os << "cd " << gcp() << path_parameters.sim_root_dir << "release/" << "\n";
        // os << gcp() << path_parameters.sim_root_dir << "/" << Executer::executable_name << " " <<  path_parameters.mode_type << " " << path_parameters.files_dir << std::endl;
        os << gcp() << path_parameters.sim_root_dir << "release/" << Executer::executable_name << " " \
           << path_parameters.mode_type << " " << path_parameters.files_dir << " " << path_parameters.sim_root_dir << " " \
           << path_parameters.rel_path;
        if(additional_args.size() != 0)
            for(auto additional_arg: additional_args)
                os << " " << additional_arg;
        os << std::endl;
    }

    void run_execution_on_cpu_cluster() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string filename = path_parameters.mode_type + "_" + path_parameters.files_dir;
        std::string qsubfile = gcp() + path_parameters.sim_root_dir + "/" + path_parameters.get_rel_cpu_bash_script_path() + "run_" + filename + ".sh";
        std::string progoutfile = gcp() + path_parameters.sim_root_dir + "/" + path_parameters.get_rel_cpu_bash_script_path() + "output_run_" + filename + ".txt";
        std::string progerrfile = gcp() + path_parameters.sim_root_dir + "/" + path_parameters.get_rel_cpu_bash_script_path() + "error_run_" + filename + ".txt";
        int res;
        if(Executer::cluster_mode == "on_cluster")
            res = system(("qsub -o " + progoutfile + " -e " + progerrfile + " " + qsubfile).c_str()); // submit the process to queue
        else
            res = system(("nice -n 17 bash " + qsubfile + " > " + progoutfile + " 2> " + progerrfile).c_str()); // execute processes on local system
        std::cout << "System code " << res << std::endl;
    }

    void prepare_execution_on_gpu_cluster() {
        std::cout << "To be implemented" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    void run_execution_on_gpu_cluster() {
        std::cout << "To be implemented" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    template <typename SBP>
    void main(/*SBP &simparams*/)
    {
        ExecutionMode mode;
        // json config_file;
        bool in_preparation;

        // Load config_file from file if present
        if (Parameters::check_if_parameter_file_exists(path_parameters.get_rel_config_path() + "/",
                                                       path_parameters.mode_type + "_params",
                                                       path_parameters.rel_path)) {
            std::cout << "Mode = " << path_parameters.mode_type << " will be executed based on provided json file"
                      << std::endl;
            // config_file = Parameters::read_parameter_file(path_parameters.get_rel_config_path() + "/", path_parameters.mode_type + "_params", path_parameters.rel_path);
            mode = mode_resolver.at(path_parameters.mode_type);
            in_preparation = false;
            // Can probably be shifted to somewhere else
            /* if(config_file["mode_type"].get<std::string>() != path_parameters.mode_type)
            {
                std::cout << "\nERROR: Mode in config file " << config_file["mode_type"].get<std::string>()
                          << "_params and mode in execution command " << path_parameters.mode_type
                          << " do not coincide" << std::endl;
                std::exit(EXIT_FAILURE);
            } */
        } else {
            std::cout << "A default " << path_parameters.mode_type << "_params.json file will be generated in configs/"
                      << path_parameters.files_dir
                      << "/ together with other parameter files. Adapt it to your considered set of equations and run the execution command again."
                      << std::endl;
            mode = mode_resolver.at(path_parameters.mode_type);
            in_preparation = true;
        }

              switch (mode) {
                  case expectation_value:
                  {
                      if(in_preparation) {
                          prep_default_execution<ExpectationValueParameters, SBP>(path_parameters);
                      }
                      else
                      {
                          SimulationParameters<SBP, ExpectationValueParameters> simparams = SimulationParameters<SBP, ExpectationValueParameters>::generate_simulation_from_file(path_parameters.get_rel_config_path(), path_parameters.mode_type);
                          Simulation<SBP, ExpectationValueParameters> sim(simparams);
                          sim.run();

                          FILE* file;
                          auto args = prepare_python_script("expectation_value");
                          PySys_SetArgv(args.first, args.second);
                          file = fopen((Executer::get_python_scripts_path() + "/modes/expectation_value.py").c_str(), "r");
                          PyRun_SimpleFile(file, "expectation_value.py");
                          fclose(file);
                          auto cwd = gcp();
                          PyRun_SimpleString(("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

                          // PyRun_SimpleString("os.chdir('cu_work_dir')");
                      }
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
            case correlation_time:
            {
                if(in_preparation) {
                    prep_default_execution<CorrelationTimeParameters, SBP>(path_parameters);
                }
                else
                {
                    SimulationParameters<SBP, CorrelationTimeParameters> simparams = SimulationParameters<SBP, CorrelationTimeParameters>::generate_simulation_from_file(path_parameters.get_rel_config_path(), path_parameters.mode_type);
                    Simulation<SBP, CorrelationTimeParameters> sim(simparams);
                    sim.run();

                    FILE* file;
                    auto args = prepare_python_script("correlation_time");
                    PySys_SetArgv(args.first, args.second);
                    file = fopen((Executer::get_python_scripts_path() + "/modes/correlation_time.py").c_str(), "r");
                    PyRun_SimpleFile(file, "correlation_time.py");
                    fclose(file);
                    auto cwd = gcp();
                    PyRun_SimpleString(("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

                    // PyRun_SimpleString("os.chdir('cu_work_dir')");
                    break;
                }
            }
            case equilibriate:
            {
                if(in_preparation) {
                    prep_default_execution<EquilibriateParameters, SBP>(path_parameters);
                }
                else
                {
                    SimulationParameters<SBP, EquilibriateParameters> simparams = SimulationParameters<SBP, EquilibriateParameters>::generate_simulation_from_file(path_parameters.get_rel_config_path(), path_parameters.mode_type);
                    Simulation<SBP, EquilibriateParameters> sim(simparams);
                    sim.run();

                    FILE* file;
                    auto args = prepare_python_script("equilibriate");
                    PySys_SetArgv(args.first, args.second);
                    file = fopen((Executer::get_python_scripts_path() + "/modes/equilibriate.py").c_str(), "r");
                    PyRun_SimpleFile(file, "equilibriate.py");
                    fclose(file);
                    auto cwd = gcp();
                    PyRun_SimpleString(("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

                    // PyRun_SimpleString("os.chdir('cu_work_dir')");
                    break;
                }
            };
            case plot_site_distribution:
            {
                if(in_preparation) {
                    prep_default_execution<PlotSiteDistributionParameters, SBP>(path_parameters);
                }
                else
                {
                    FILE* file;
                    auto args = prepare_python_script("plot_site_distribution");
                    PySys_SetArgv(args.first, args.second);
                    file = fopen((Executer::get_python_scripts_path() + "/modes/plot_site_distribution.py").c_str(), "r");
                    PyRun_SimpleFile(file, "plot_site_distribution.py");
                    fclose(file);
                    auto cwd = gcp();
                    PyRun_SimpleString(("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

                    break;
                }
            }
            default:
                std::cout << "mode not known..." << std::endl;
                break;
        }
    }

    std::pair<int, wchar_t**> prepare_python_script(std::string python_file)
    {
        int argc = 2;
        const char * argv[2];

        argv[0] = python_file.c_str();
        argv[1] = path_parameters.files_dir.c_str();
        PyRun_SimpleString(("import os; os.chdir('" + gcp() + path_parameters.sim_root_dir + "')").c_str());

        // argv[2] = path_parameters.sim_root_dir.c_str(); -> go to current sim_root_dir
        /* if(path_parameters.rel_path)sdf
            argv[3] = "True";
        else
            argv[3] = "False"; */

        auto** _argv = (wchar_t**) PyMem_Malloc(sizeof(wchar_t*)*argc);
        for (int i=0; i<argc; i++) {
            wchar_t* arg = Py_DecodeLocale(argv[i], NULL);
            _argv[i] = arg;
        }

        return std::pair<int, wchar_t**> {argc, _argv};
    }

    static std::string get_python_scripts_path()
    {
        return Executer::python_scripts_path;
    }

    static void set_python_scripts_path(const std::string python_scripts_path_)
    {
        Executer::python_scripts_path = python_scripts_path_;
        std::cout << "Setting python scripts path to " << python_scripts_path_ << std::endl;
    }

    static std::string get_executable_name()
    {
        return Executer::executable_name;
    }

    static void set_executable_name(const std::string executable_name_)
    {
        Executer::executable_name = executable_name_;
        std::cout << "Setting executable name to " << Executer::executable_name << std::endl;
    }

    static std::string get_cluster_mode()
    {
        return Executer::cluster_mode;
    }

    static void set_cluster_mode(const std::string cluster_mode_)
    {
        Executer::cluster_mode = cluster_mode_;
        std::cout << "Setting cluster mode to " << Executer::cluster_mode << std::endl;
    }

    static void set_conda_activate_path(const std::string conda_activate_path_)
    {
        Executer::conda_activate_path = conda_activate_path_;
        std::cout << "Setting conda activate path to " << Executer::conda_activate_path << std::endl;
    }

    static void set_virtual_env(const std::string virtual_env_)
    {
        Executer::virtual_env = virtual_env_;
        std::cout << "Setting virtual_env to " << Executer::virtual_env << std::endl;
    }


    const PathParameters path_parameters;
};

template <typename SBP>
void run_from_file(int argc, char **argv)
{
    std::string mode_type = std::string(argv[1]);
    std::string files_dir = std::string(argv[2]);
    std::string sim_root_dir = "/./";
    bool rel_path = true;
    if(argc > 3)
        sim_root_dir = std::string(argv[3]);
    if(argc > 4)
    {
        std::string rel_path_boolean = std::string(argv[4]);
        if (rel_path_boolean == "false")
            rel_path = false;
    }

    std::cout << "mode_type: " << mode_type << std::endl;
    std::cout << "files_dir: " << files_dir << std::endl;
    std::cout << "sim_root_dir: " << sim_root_dir << std::endl;
    std::cout << "rp: " << rel_path << "\n\n" << std::endl;

    PathParameters path_parameters(mode_type, files_dir, sim_root_dir, rel_path);
    Executer executer(path_parameters);
    executer.main<SBP>();
}

template <typename SBP>
void execute(const std::string mode_type, const std::string files_dir, const std::string sim_root_dir="/./",
             const bool rel_path=true, const Executer::RunningMode running_mode=Executer::local, const bool execute_code=true,
             const std::vector<std::string> additional_args={})
{
    PathParameters path_parameters(mode_type, files_dir, sim_root_dir, rel_path);
    Executer executer(path_parameters);
    if(running_mode == Executer::local and execute_code)
        // Run based on target name - Does the same as ./{ProjectName} {ExecutionMode} {Directory} - might also call directly the respective main function - exception!
        executer.main<SBP>();
    else if(running_mode == Executer::on_cpu_cluster) {
        executer.prepare_execution_on_cpu_cluster(additional_args);
        if(execute_code)
            executer.run_execution_on_cpu_cluster();
    }
    else if(running_mode == Executer::on_gpu_cluster) {
        executer.prepare_execution_on_gpu_cluster();
        if(execute_code)
            executer.run_execution_on_gpu_cluster();
    }
}

template<typename ExecutionParams, typename SBP>
void Executer::prep_default_execution(const PathParameters path_parameters)
{
    ExecutionParams expectation_value_parameters(json {});
    expectation_value_parameters.write_to_file(path_parameters.get_rel_config_path());

    SimulationParameters<SBP, ExecutionParams> simparams = SimulationParameters<SBP, ExecutionParams>::generate_simulation_from_mode(
            path_parameters.get_rel_config_path(),
            path_parameters.get_rel_data_path(),
            path_parameters.mode_type);
}

// ./{Main} {mode_type} {files_dir} {sim_root_dir="/data/"} {rel_path=true}

#endif //MAIN_EXECUTER_HPP
