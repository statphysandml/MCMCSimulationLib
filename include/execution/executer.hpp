//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXECUTER_HPP
#define MAIN_EXECUTER_HPP

#include <Python.h>

#include <iostream>
#include <map>
#include <utility>

#include "param_helper/params.hpp"
#include "../mcmc_simulation/util/path_parameters.hpp"
#include "../mcmc_simulation/simulation.hpp"

#include "modes/expectation_value.hpp"


std::string get_python_scripts_path();
void initialize_python();
void finalize_python();


struct Executer
{
    enum ExecutionMode {expectation_value, correlation_time, plot_site_distribution};
    static const std::map< std::string, ExecutionMode> mode_resolver;

    Executer(const PathParameters path_parameters_) : path_parameters(path_parameters_)
    /* const std::string mode_name_, const std::string filename_, const std::string dir_, const std::string root_dir_, const bool rel_path_ = true) :
    mode_name(mode_name_), filename(filename_), dir(dir_), root_dir(root_dir_), rel_path(rel_path_), mode(mode_resolver.at(mode_name)) */
    {}

    static std::string mode_to_string (ExecutionMode mode_) {
        switch (mode_) {
            case expectation_value:
                return "expectation_value";
            case correlation_time:
                return "correlation_time";
            case plot_site_distribution:
                return "plot_site_distribution";
            default:
                return "mode_not_known";
        }
    }

    /* static void exec_visualization(const VisualizationParameters& visualization_parameters, std::string dir); */
    static void prep_exec_expectation_value(const PathParameters path_parameters);

    template <typename SBP>
    void main()
    {
        ExecutionMode mode;
        json config_file;
        bool in_preparation;

        // Load config_file from file if present
        if(Parameters::check_if_parameter_file_exists(path_parameters.get_rel_config_path() + "/",
                                                      path_parameters.mode_type + "_params", path_parameters.rel_path))
        {
            std::cout << "Mode = " << path_parameters.mode_type << " will be executed based on provided json file" << std::endl;
            config_file = Parameters::read_parameter_file(path_parameters.get_rel_config_path() + "/",
                                                          path_parameters.mode_type + "_params", path_parameters.rel_path);
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
        }
        else
        {
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
                    prep_exec_expectation_value(path_parameters);
                    SimulationParameters<SBP, ExpectationValueParameters> simparams = SimulationParameters<SBP, ExpectationValueParameters>::generate_simulation_from_mode(
                            path_parameters.get_rel_config_path(),
                            path_parameters.get_rel_data_path(),
                            path_parameters.mode_type);
                }
                else
                {
                    SimulationParameters<SBP, ExpectationValueParameters> simparams = SimulationParameters<SBP, ExpectationValueParameters>::generate_simulation_from_file(path_parameters.get_rel_config_path(), path_parameters.mode_type);
                    Simulation<SBP, ExpectationValueParameters> sim(simparams);
                    sim.run();

                    FILE* file;
                    auto args = prepare_python_script("expectation_value");
                    PySys_SetArgv(args.first, args.second);
                    file = fopen((get_python_scripts_path() + "/modes/expectation_value.py").c_str(), "r");
                    // file = fopen("./../python_scripts/modes/expectation_value.py","r");
                    // file = fopen("expectation_value.py","r");
                    PyRun_SimpleFile(file, "expectation_value.py");
                    fclose(file);
                    auto cwd = gcp();
                    PyRun_SimpleString(("import os; os.chdir('" + cwd.substr(0, cwd.size() - 3) + "/cmake/')").c_str());

                    // PyRun_SimpleString("os.chdir('cu_work_dir')");
                    break;
/*                    VisualizationParameters visualization_parameters(config_file, path_parameters);
                    exec_visualization(visualization_parameters, dir); */
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
                /* SimulationParameters<SystemBaseParams, CorrelationTimeParameters> simparams(filename, dir, mode_name);
                Simulation<SystemBaseParams, CorrelationTimeParameters> sim(simparams);
                sim.single_run();

                FILE* file;
                auto args = prepare_python_script("correlation_time");
                PySys_SetArgv(args.first, args.second);
                file = fopen("./../python_scripts/modes/correlation_time.py","r");
                PyRun_SimpleFile(file, "correlation_time.py");
                fclose(file);
                break; */
            }
            case plot_site_distribution:
            {
                /* FILE* file;
                auto args = prepare_python_script("plot_site_distribution");
                PySys_SetArgv(args.first, args.second);
                file = fopen("./../python_scripts/modes/plot_site_distribution.py","r");
                PyRun_SimpleFile(file, "plot_site_distribution.py");
                fclose(file);
                break; */
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

    /* const std::string mode_name;
    const std::string filename;
    const std::string dir;
    const std::string root_dir;
    const bool rel_path;
    const ExecutionMode mode; */
    const PathParameters path_parameters;
};


// ./{Main} {mode_type} {files_dir} {sim_root_dir="/data/"} {rel_path=true}

template <typename SBP>
void run_from_file(int argc, char **argv)
{
    std::string mode_type = std::string(argv[1]);
    std::string files_dir = std::string(argv[2]);
    std::string sim_root_dir = "/./";
    bool rel_path = true;
    if(argc > 3) {
        sim_root_dir = std::string(argv[3]);
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
void execute(const std::string mode_type, const std::string files_dir, const std::string sim_root_dir="/./", const bool rel_path=true)
{
    PathParameters path_parameters(mode_type, files_dir, sim_root_dir, rel_path);
    Executer executer(path_parameters);
    executer.main<SBP>();
}



#endif //MAIN_EXECUTER_HPP
