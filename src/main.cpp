#include "../include/mcmc_simulation/header.hpp"

#include "../include/execution/modes/expectation_value.hpp"
#include "../include/execution/modes/correlation_time.hpp"

class SystemBaseTemplate;

class SystemBaseTemplateParameters : public SystemBaseParameters {
public:
    SystemBaseTemplateParameters(const json params, const std::string rel_config_path_=""): SystemBaseParameters(params)
    {}

    std::unique_ptr<SystemBaseTemplate> generate() { return std::make_unique<SystemBaseTemplate>(*this); }

    /* static const std::string name() // Optional
    {
        return "ising_model_system_params";
    }

    void write_to_file(const std::string& root_dir) { // Optional - Needs to be defined if name() function is defined
        Parameters::write_to_file(root_dir, name());
    } */

private:
    friend class SystemBaseTemplate;
};


class SystemBaseTemplate : public SystemBase<SystemBaseTemplate>
{
public:
    explicit SystemBaseTemplate(const SystemBaseTemplateParameters &imsp_) : imsp(imsp_)
    {}

    void update_step(uint measure_interval=1)
    {}

    void initialize(std::string starting_mode)
    {}

    auto get_size() const
    {
        return lattice.size();
    }

    auto at(int i) const
    {
        return lattice[i];
    }

    auto& at(int i)
    {
        return lattice[i];
    }

    auto get_system_representation() const
    {
        return lattice;
    }

    auto& get_system_representation()
    {
        return lattice;
    }

    // Optional - For standard measurements, the already implemented method can be used
    std::vector<std::string> perform_measure() override
    {
        std::vector<std::string> results;
        results.push_back(std::to_string(0.0));
        return results;
    }

    // Optional - For standard measurements, the already implemented method can be used
    std::vector<std::string> get_measure_names() override
    {
        std::vector<std::string> measure_names;
        measure_names.emplace_back("default_measure");
        return measure_names;
    }


private:
    // Can be replaced by any other representation
    std::vector<double> lattice;

    const SystemBaseTemplateParameters &imsp;
};


int main() {
    // Setting up execution parameters

    typedef ExpectationValueParameters ExecutionParams;
    ExecutionParams execution_parameters(100, 10000, 100, {}, {});

    // Setting up simulation parameters

    std::string target_name = "IsingModel";
    std::string rel_data_path = "/data/" + target_name + "/";

    SystemBaseTemplateParameters ising_model_system_params(json {});

    // Setting up the actual simulation

    auto simulation_params = SimulationParameters< SystemBaseTemplateParameters , ExecutionParams >::generate_simulation(
            ising_model_system_params, execution_parameters, rel_data_path);

    // Optional - Store simulation parameters as config files for a possible later simulation
    std::string rel_config_path = "/configs/" + target_name + "/";
    simulation_params.write_to_file(rel_config_path);
    std::cout << "All necessary parameters for the simulation have been written to rel_config_path" << std::endl;

    // Load simulation parameters from file
    auto from_file_simulation_params = SimulationParameters< SystemBaseTemplateParameters , ExecutionParams >::generate_simulation_from_file(rel_config_path);

    Simulation< SystemBaseTemplateParameters, ExecutionParams > simulation(from_file_simulation_params);
    simulation.run();

    typedef CorrelationTimeParameters ExecutionParams2;
    ExecutionParams2 execution_parameters2(10, 100, 100, {"Mean"});
    execution_parameters2.write_to_file(rel_config_path);

    // Load simulation parameters from file
    auto from_mode_simulation_params = SimulationParameters< SystemBaseTemplateParameters , ExecutionParams2 >::generate_simulation_from_file(rel_config_path);

    Simulation< SystemBaseTemplateParameters, ExecutionParams2 > simulation2(from_mode_simulation_params);
    simulation2.run();


    // Running a simulation from file


    // Running a simulation from file with a different mode

    return 0;

    // ToDo: Explain different methods to run a simulation (constructors)

    // ToDo: Generate additional template for simulation with measurements

    // ToDo: Generate example with Ising model

    // ToDo: Add example on how to use the executer // from file, on cluster, etc.

    // ToDo: Add example with custom executer



    // ToDo: General ToDo: - Check that running parameter works!
    //  - Adapt lattice to not depend on rel_config anymore
    //  - Think about adding parameters for setting certain paths explicitly
}