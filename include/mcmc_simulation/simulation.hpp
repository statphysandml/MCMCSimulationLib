#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>

// #include <boost/filesystem.hpp>

#include "util/mcmc_parameter_functions.hpp"
#include "markov_chain.hpp"

#include "param_helper/json.hpp"
#include "param_helper/fileos.hpp"

using json = nlohmann::json;

json::iterator get_running_parameter_iterator(json& params, std::vector<std::string> running_parameter_path);

json update_running_parameter(const json params, std::vector<std::string> running_parameter_path, double new_running_parameter);

template <typename SBP, typename EP>
class SimulationParameters : public Parameters {
public:
    // Constructor for loading from params
    explicit SimulationParameters(const json params_, const std::string& mode_, const bool traceable_=true) : Parameters(params_), mode(mode_), traceable(traceable_)
    {
        // measure_interval = get_value_by_key("measure_interval");

        // *** SIMULATION PARAMETERS ***
        rel_config_path = get_value_by_key<std::string>("rel_config_path");
        /* if(rel_config_path == "None")
            traceable = false; */
        rel_data_path = get_value_by_key<std::string>("rel_data_path");

        // Optionally
        running_parameter_kind = get_value_by_key<std::string>("running_parameter_kind", "None");
        running_parameter = get_value_by_key<std::string>("running_parameter", "None");
        rp_minimum = get_value_by_key<double>("rp_minimum", 0);
        rp_maximum = get_value_by_key<double>("rp_maximum", 0);
        rp_number = get_value_by_key<int>("rp_number", 0);


        // *** DIRECTORIES *** -> ToDo: Problems if config file is loaded from somewhere else?
        // Create folder in data directory if not present
        if(!boost::filesystem::is_directory(gcp() + rel_data_path))
        {
            std::cout << "Create root directory" << std::endl;
            boost::filesystem::create_directories(gcp() + rel_data_path);
        }
        else
        {
            /* std::cout << "Clear data directory" << std::endl;
            boost::filesystem::path path_to_remove(gcp() + rel_data_path);
            for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it) {
                boost::filesystem::remove_all(it->path());
            } */
        }

        // Create folder in config directory if not present and if simulation should be traceable
        if(traceable and !boost::filesystem::is_directory(gcp() + rel_config_path))
        {
            std::cout << "Create config directory" << std::endl;
            boost::filesystem::create_directories(gcp() + rel_config_path);
        }

        // *** SYSTEM BASE PARAMETERS *** (default, from file, from json object)
        systembase_params = std::make_unique<SBP>(
                generate_parameter_class_json<SimulationParameters<SBP, EP>, SBP> (*this, systembase_params->param_file_name(), rel_config_path),
                        rel_config_path);

        // *** EXECUTION PARAMETERS ***
        // Load execution parameters from params object -> enables to use
        std::string execution_params_path = get_value_by_key<std::string>("execution_params_path", rel_config_path);
        if(haskey("execution_params")) {// !boost::filesystem::is_regular_file(gcp() + execution_params_path + "/" + mode + "_params.json")) {
            execution_params = std::make_unique<EP>(get_value_by_key<json>("execution_params"));
            delete_entry("execution_params");
            if(traceable)
                execution_params->write_to_file(execution_params_path);
        }
        // Load from params object
        /* else if()
        {

        } */
        // Load execution parameters from file
        else {
            std::cout << "Load execution parameters from file based on given mode" << std::endl;
            execution_params = std::make_unique<EP>(
                    Parameters::read_parameter_file(execution_params_path, mode + "_params"));
        }

        // *** MEASURES ***
        // Temporarily update measures if wanted
        auto &execution_params_measures = execution_params->get_measures();
        if(!execution_params_measures.empty())
            systembase_params->update_measures(execution_params_measures);

        // *** RUNNING PARAMETER ***
        if(running_parameter != "None")
        {
            // Construct running parameter path for later simulations
            auto expanded_parameters = build_expanded_raw_parameters();
            // std::cout << expanded_parameters << std::endl;
            running_parameter_path = std::vector<std::string> {};
            auto running_parameter_kind_path_found = construct_parameter_path(expanded_parameters.get_json(), running_parameter_kind, running_parameter_path);
            if(!running_parameter_kind_path_found)
            {
                std::cout << "Running parameter kind not found" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            running_parameter_path.push_back(running_parameter);
        }

        if(traceable)
        {
            this->write_to_file();
            std::cout << "Generated all necessary simulation parameter config files for the computation of " + mode
                      << ". Run simulation in a console by running/rerunning" << std::endl;
            std::cout << "\n#############################################\n./{Executable} " << mode <<
                      " " << rel_config_path.substr(9, rel_config_path.size() - 9) << "\n#############################################\n" << std::endl;
        }
    }

    // Constructor for generating an entire simulation WITHOUT the storage of the config files
    static SimulationParameters generate_simulation(
            SBP& systembase_params_,
            EP& execution_params_,
            const std::string& rel_data_path_,
            const std::string& running_parameter_kind_="None",
            const std::string& running_parameter_="None",
            const double& rp_minimum_=0,
            const double& rp_maximum_=0,
            const double& rp_number_=0
    )
    {
        return SimulationParameters(
                json {
                        {"rel_config_path", "None"},
                        {"rel_data_path", rel_data_path_},
                        {"running_parameter_kind", running_parameter_kind_},
                        {"running_parameter", running_parameter_},
                        {"rp_minimum", rp_minimum_},
                        {"rp_maximum", rp_maximum_},
                        {"rp_number", rp_number_},
                        {systembase_params_.param_file_name(), systembase_params_.get_json()},
                        {"execution_params", execution_params_.get_json()}
                },
                EP::name(),
                false
        );
    }

    // Constructor for generating an entire simulation WITH the storage of the config files
    static SimulationParameters generate_traceable_simulation(
            SBP& systembase_params_,
            EP& execution_params_,
            const std::string& rel_config_path_,
            const std::string& rel_data_path_,
            const std::string& running_parameter_kind_="None",
            const std::string& running_parameter_="None",
            const double& rp_minimum_=0,
            const double& rp_maximum_=0,
            const double& rp_number_=0
    )
    {
        return SimulationParameters(
                json {
                        {"rel_config_path", rel_config_path_},
                        {"rel_data_path", rel_data_path_},
                        {"running_parameter_kind", running_parameter_kind_},
                        {"running_parameter", running_parameter_},
                        {"rp_minimum", rp_minimum_},
                        {"rp_maximum", rp_maximum_},
                        {"rp_number", rp_number_},
                        {systembase_params_.param_file_name(), systembase_params_.get_json()},
                        {"execution_params", execution_params_.get_json()}
                },
                EP::name()
        );

        // ToDo: Is it better to remove systembase and execution afters for this case - for consistency with a pure run from file?
    }

    // Constructor that uses existing mode parameters and only refers to generally set simulation parameters
    static SimulationParameters generate_simulation_from_mode(
            const std::string& rel_config_path_,
            const std::string& rel_data_path_,
            const std::string& mode_,
            const std::string& running_parameter_kind_="None",
            const std::string& running_parameter_="None",
            const double rp_minimum_=0.0,
            const double rp_maximum_=0.0,
            const int rp_number_=0.0
    )
    {
        return SimulationParameters(
                json {
                        {"rel_config_path", rel_config_path_},
                        {"rel_data_path", rel_data_path_},
                        {"running_parameter_kind", running_parameter_kind_},
                        {"running_parameter", running_parameter_},
                        {"rp_minimum", rp_minimum_},
                        {"rp_maximum", rp_maximum_},
                        {"rp_number", rp_number_}
                },
                mode_,
                true
        );
    }

    // Constructor for loading from file
    static SimulationParameters generate_simulation_from_file(const std::string& rel_config_path_, const std::string& mode_)
    {
        auto params_ = Parameters::read_parameter_file(rel_config_path_, param_file_name());
        return SimulationParameters(params_, mode_, false);
    }

    void initialize_simulation_parameters(const double rp=0)
    {
        if(running_parameter != "None") {
            markovchain_params = execution_params->generate_markovchain_params(running_parameter, rp);
            update_system_params_by_new_running_parameter(rp);
        }
        else {
            markovchain_params = execution_params->generate_markovchain_params();
        }
    }

    void write_to_file() {
        json systembase_params_ = systembase_params->get_json();
        delete_entry(systembase_params->param_file_name());
        std::string systembase_params_path = get_value_by_key<std::string>(systembase_params->param_file_name() + "_path", rel_config_path);
        systembase_params->write_to_file(systembase_params_path);

        Parameters::write_to_file(rel_config_path, param_file_name());
        add_entry(systembase_params->param_file_name(), systembase_params_);

    }

    Parameters build_expanded_raw_parameters()
    {
        Parameters parameters = Parameters::create_by_params(params);
        parameters.add_entry(systembase_params->param_file_name(), systembase_params->build_expanded_raw_parameters().get_json());
        parameters.add_entry("execution_params", execution_params->build_expanded_raw_parameters().get_json());
        return parameters;
    }

    void print() {
        systembase_params->print_something();
    }

    void update_system_params_by_new_running_parameter(double new_running_parameter) {
        auto expanded_parameters = build_expanded_raw_parameters().get_json();
        json new_system_params = update_running_parameter(
                expanded_parameters, running_parameter_path, new_running_parameter);
        systembase_params = std::make_unique<SBP>(new_system_params[systembase_params->param_file_name()], rel_config_path);
    }

    static const std::string param_file_name()
    {
        return "sim_params";
    }

private:
    template <typename, typename>
    friend class Simulation;

    // std::string systembase_name;
    std::unique_ptr<SBP> systembase_params; // -> do these really have to be pointer?
    std::unique_ptr<EP> execution_params;
    std::string rel_config_path;
    std::string rel_data_path;
    std::string mode;
    std::string running_parameter_kind;
    std::string running_parameter;
    std::vector<std::string> running_parameter_path;
    double rp_minimum;
    double rp_maximum;
    int rp_number;

    std::unique_ptr<MarkovChainParameters> markovchain_params;

    // bool from_file = false;
    const bool traceable;
};


template <typename SBP, typename EP>
class Simulation {
public:
    explicit Simulation(SimulationParameters<SBP, EP> &mp_) : mp(mp_)
    {}

    void run() {
        if(mp.running_parameter == "None")
            single_run(mp.mode);
        else
        {
            for (double rp = mp.rp_minimum; rp <= mp.rp_maximum; rp += (mp.rp_maximum - mp.rp_minimum) / mp.rp_number) {
                single_run(mp.mode + "_" + mp.running_parameter + "=" + std::to_string(rp), rp);
            }
        }
    }

    void single_run(std::string filename, const double rp=0) {
        mp.initialize_simulation_parameters(rp);

        write_setting_file(mp.rel_data_path, filename);

        Fileos os (gcp() + mp.rel_data_path + "/" + filename  + ".dat");
        MarkovChain<SBP> mc(*mp.markovchain_params, *mp.systembase_params, os.get());
        mc.run();
    }

private:
    SimulationParameters<SBP, EP> &mp;

    // To get explicit informations about the particular run? It might be reasonable to add expanded files to the output -> do this!
    void write_setting_file(const std::string& rel_config_path, const std::string &filename) const
    {
        auto raw_parameters = mp.build_expanded_raw_parameters();
        raw_parameters.write_to_file(rel_config_path, filename);
    }
};

#endif