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
        mcmc::measures::ReadableMeasure readable_measures(this->path_parameters.get_rel_data_path());


        auto simulation = mcmc::simulation::Simulation<IsingModel>::prepare_simulation_from_file(
            system_params, readable_measures,
            "systembase_params", "beta", mcmc::util::linspace(0.1, 0.7, 6));

        typedef mcmc::mode::EquilibriumTime EquilibriumTimeParams;
        EquilibriumTimeParams equilibrium_time_parameters(100, 1000, 0.1, 10, "Mean");

        typedef mcmc::mode::CorrelationTime CorrelationTimeParams;
        CorrelationTimeParams correlation_time_parameters(1000, 400, this->path_parameters.get_rel_results_path(), "Mean");

        typedef mcmc::mode::ExpectationValue ExpectationValueParams;
        ExpectationValueParams expectation_value_parameters(
            this->path_parameters.get_rel_results_path(), 20000, this->path_parameters.get_rel_results_path(), {"AbsMean", "SecondMoment", "Mean", "Config"}, {"Energy"}, "hot", "statistical");

        // Store simulation parameters
        simulation.write_to_file(this->path_parameters.get_rel_config_path());
        equilibrium_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());
        correlation_time_parameters.write_to_file(this->path_parameters.get_rel_config_path());
        expectation_value_parameters.write_to_file(this->path_parameters.get_rel_config_path());
    }
};


int main(int argc, char **argv) {
    // Initialize project dependent parameters
    param_helper::proj::set_relative_path_to_project_root_dir("../");

#ifdef PYTHON_BACKEND
    mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
#endif

    CmdIntSimulation cmdint_simulation("IsingModelSimulation", "./", true);
    cmdint_simulation.main(argc, argv);

    // Finalization
#ifdef PYTHON_BACKEND
    mcmc::util::finalize_python();
#endif
    return 0;
}
