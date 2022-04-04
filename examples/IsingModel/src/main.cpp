#include "../include/ising_model/config.h"

#include <mcmc_simulation/header.hpp>
#include <mcmc_simulation/util/intervals.hpp>
#include <modes/mode_header.hpp>
#include <mcmc_simulation/util/random.hpp>

#include <command_line_support/cmdint.hpp>

#include "../include/ising_model/ising_model.hpp"


struct CmdIntSimulation : mcmc::cmdint::CmdIntSim<IsingModel, mcmc::measures::ReadableMeasure>
{
    using mcmc::cmdint::CmdIntSim<IsingModel, mcmc::measures::ReadableMeasure>::CmdIntSim;

    void prepare() override
    {
        IsingModel system_params(0.4, 1.0, 0.0, {4, 4});
        mcmc::measures::ReadableMeasure readable_measures(this->path_parameters.get_rel_data_dir());


        auto simulation = mcmc::simulation::Simulation<IsingModel>::prepare_simulation_from_file(
            system_params, readable_measures,
            "systembase_params", "beta", mcmc::util::linspace(0.1, 0.7, 7));

        typedef mcmc::mode::EquilibriumTime EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(100, 1000, 0.1, 10, "Mean");

        typedef mcmc::mode::CorrelationTime CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(1000, 400, this->path_parameters.get_rel_results_dir(), "Mean");

        typedef mcmc::mode::ExpectationValue ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
            this->path_parameters.get_rel_results_dir(), 20000, this->path_parameters.get_rel_results_dir(), {"AbsMean", "SecondMoment", "Mean", "Config"}, {"Energy"}, "hot", "statistical");

        // Store simulation parameters
        simulation.write_to_file(this->path_parameters.get_rel_config_dir());
        equilibrium_time_parameters.write_to_file(this->path_parameters.get_rel_config_dir());
        correlation_time_parameters.write_to_file(this->path_parameters.get_rel_config_dir());
        expectation_value_parameters.write_to_file(this->path_parameters.get_rel_config_dir());
    }
};


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif
    
    /* Example usage of mcmc::cmdint::simulation, where the prepare() method is used to specify the simulation configuration parameters.
    * 
    * The simulation parameters need to be written into a first to file by running the executable without any further arguments:
    * 
    * ./IsingModel
    * 
    * Afterwards, additional arguments can be used to execute the respective simulation modes, where the syntax is:
    * 
    * ./{executable} {execution_mode} {target_name} {sim_root_dir} {rel_path} {run} {eval}
    * 
    * Based on the given example, the next commands for executing the simulations are respectively:
    * 
    * ./IsingModel equilibrium_time IsingModelSimulation "./" true true true
    * 
    * for a computation of the times to equilibrum,
    * 
    * ./IsingModel correlation_time IsingModelSimulation "./" true true true
    * 
    * for a computation of the correlation times, and
    * 
    * ./IsingModel expectation_value IsingModelSimulation "./" true true true
    * 
    * for a computation of the expectation values.
    */
    CmdIntSimulation cmdint_simulation("IsingModelSimulation", "./", true);
    cmdint_simulation.main(argc, argv);

    /* Note that is also possible to directly call mcmc::cmdint::simulation. In this case, the default constructors are used to
    * write default configuration files to file. After adapting those to the desired settings, the simulation can be executed
    * in the same manner as above:
    */
    // mcmc::cmdint::CmdIntSim<IsingModel, mcmc::measures::ReadableMeasure> cmdint_simulation("IsingModelSimulation", "./", true);
    // cmdint_simulation.main(argc, argv);

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
