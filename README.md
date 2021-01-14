MCMCSimulationLib
=================

MCMCSimulationLib is a C++ library that simplifies setting up a Markov Chain Monte Carlo algorithms. The library provides a generic framework to perform simulations for standard tasks like a computation of expectation values or a determination of the autocorrelation time. Certain base classes enable an easy implementation of new theories and models.

An additional feature of the library is that all hyperparameters of a simulation can be saved and reused for the same simulation at a later point. Due to this, the library can be used to manage and start computations on a cluster pretty easy.

The computation of numerical results is implemented in Python. The respective library can be found here: https://github.com/statphysandml/MCMCEvaluationLib. Besides an evaluation of the data, it is possible to load all configurations as a PyTorch dataset.

In short, the library takes care of all the annoying parts of a Markov Chain Monte Carlo algorithm and leaves to you the implementation of the actual considered problem.

Build
-----

Certain configuration parameters need to be defined for an execution of the C++ program. They need to be defined in a config.sh file in the build/ directory and in a project_config.sh file in the bash_scripts/ directory. The config.sh file contains all important information for the integration of Python into the program. So far, the library can only be used with a virtual environment. The config_template.sh file is a template where all necessary parameters are defined:
```bash
path_to_python3="~/.miniconda3/envs/virtual_env/" # (optional)
path_to_conda_activate="~/.miniconda3/bin/activate" # (optional)
virtual_env="virtual_env" # (optional)
python_version="3.7" # (optional)
```
The parameters need to be adapted to the virtual environment of the underlying system.

Using the library without any Python components is possible by leaving the config.sh file empty.

Having a config.sh file in the build directory, the library can be build with
```bash
cd build
bash build.sh
```
The file project_config.sh defines additional project-dependent parameters. They are used when a new project is generated with the build_project.sh file. There also exists a template file (project_config_template.sh) which contains all necessary parameters:
```bash
cluster_mode="local"
# (optional - default=local) local/on_cluster - Can be adapted temporarily by
# adding -DCLUSTER_MODE=".." to the cmake command
# - "local" = for testing - does not actually start the code on the cluster but locally
# and performs all the necessary preparation
# - "on_cluster" = for the actual execution on a cluster
python_modules_path="~/MCMCSimulationLib/examples/python_scripts"
# (optional - default="./python_scripts" for projects and "./../python_scripts/" for
# simulations.) for a possible execution code of custom written functions and modules.
# The directory "python_modules_path" is added to sys.path by the program.
# The path needs to be defined relative to the project root path)
```

The first parameter "cluster_mode" indicates whether the algorithms are started on the cluster (on_cluster) or locally (local).

#### cluster_mode = local

The "local" option can be used to test whether the library prepares a computation on a cluster correctly. In this case, the simulation runs locally on your machine.

#### cluster_mode = on_cluster

The option can be changed to "on_cluster". In this case the jobs are sent to the cluster. There are two functions "prepare_execution_on_cpu_cluster" and "run_execution_on_cpu_cluster" that take care of this. The functions can be found in the file src/execution/execution.cpp and need to be adapted according to the used cluster.

Both options can be updated by building the CMake files of the executable again. You might change the cluster mode, for example, by executing
```bash
cmake ../cmake/ -DCMAKE_BUILD_TYPE=Release -DCLUSTER_MODE=on_cluster
make -j4
```

in the release directory of your project. More details on how to execute a simulation on the cluster can be found in the main.cpp file of the SimulateAndExecute example (https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/simulations/SimulateAndExecute/main.cpp) or in the main.cpp file of a template project (see Template Project).

Examples
--------

Examples can be generated with
```bash
cd bash_scripts
bash build_examples.sh
```

The code compiles all examples in the examples/ directory. The directory is structured as follows. The examples/ directory is a so called project. Additional, so-called simulations can be found in the examples/simulations/ directory.

In the project, a simulation of the Ising model is implemented as an example. A so-called systembase class is defined in examples/include/ising_model.hpp. The simulation can be executed by running ./IsingModel in the examples/release/. In the example, the correlation time is computed for several values of the inverse temperature. In the second part, simulations for the computation of expectation values is executed.

Configuration files to the different simulations are located in the examples/config directory. The simulation data is stored together with one single config file for the simulation in examples/data. Plots and numerical results for the correlation time as well as computed expectation values are saved in examples/results. The data evaluation takes place in Python based on code of the MCMCEvaluationLib and the pystatplottools library. A thorough evaluation of the simulation results is shown in a jupyter notebook that can be found here: https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/jupyter_notebooks/ising_model_cheat_sheet.ipynb. All important functionalities of the MCMCEvaluationLib are used for the data evaluation. Further plots are generated with the help of the pystatplottools library (https://github.com/statphysandml/pystatplottools).

Further examples in the examples/simulations/ directory contain code for a simple simulation and demonstrate how to store and load simulation parameters. Further, it is explained in detail how a simulation can be executed within the library. The examples/python_scripts/examples directory shows in addition to the ising_model_cheat_sheet.ipynb notebook several ways to use the MCMCEvaluationLib. The library also provides functionalities to load the generated Monte Carlo configurations as a PyTorch dataset. This makes the application of further machine learning tasks on the data pretty easy.

Template Project
----------------

A new project that demonstrates the basic functionalities of the library based on a template class for a model or theory can be generated in an arbitrary directory with
```bash
cd bash_scripts
bash build_project.sh
```
A project can be used as a template for your own simulation. The main.cpp contains detailed instructions on different ways to execute the simulation. The project itself has a directory bash_scripts/ with an additional build_simulation.sh bash script. This can be used to generate a template simulation for a further executable which uses code of your project. These smaller simulation projects are very useful since they enable the generation of exeuctables for several different simulations of your projects. This enables a rerunning of a simulation at a later point and simplifies running code on a cluster. For example, one might use the following workflow:

- Write all important code in the include/ and the src/ directory of your project.
- Generate a new simulation with the build_simulation.sh file.
- Adapt the main.cpp file in your simulation for an execution of a MCMC simulation.
- Transfer the code to your cluster and rerun build_simulation.sh. Enter the name of your simulation. The bash script recognizes that the simulation already exists and just adapts the CMakeLists.txt file according to the settings in your project_config.sh file.
- Execute the code with the mcmc::exeuction::execute<>() function to submit the simulation to the cluster.

Both, the build_project.sh and the build_simulation.sh bash script recognize if a project or a simulation has been built before. If these scripts are executed with the same input, only the CMakeLists.txt file is adapted according to the settings in the project_config.sh file and with respect to the executables of the local libraries (which can be found in the external_submodules/ directory).

Usage
-----

cmake (CMakeLists.txt) - The CMakeLists.txt files of the example projects and of the template project contain examples for a possible integration of the library with CMake.

A Simple Example - Simulation of an Ising Model
--------

In this example, the mean value and other observalbes are computed for the Ising Model. The Code for the simulation is given by
```c++
// Setting up system parameters (beta, J, h, dimensions)
IsingModelParameters system_params(0.4, 1.0, 0.0, {4, 4});

/* Setting up execution parameters (number of equilibrium steps, number of measurements,
* correlation time, measures, optional measures that are computed in python) */
typedef mcmc::execution::ExpectationValueParameters ExpectationValueParams;
ExpectationValueParams execution_parameters(
    100, 10000, 100, {"AbsMean", "SecondMoment", "Mean", "Config"}, {"Energy"}
);

// Setting up simulation parameters
std::string target_name = "IsingModelSimulation";
std::string rel_data_path = "/data/" + target_name + "/";
auto simulation_params = mcmc::simulation::SimulationParameters<
    IsingModelParameters, ExpectationValueParams>::generate_simulation(
        system_params, execution_parameters, rel_data_path, "systembase_params", "beta", 0.1, 0.7, 6
);

// Store simulation parameters
simulation_params.write_to_file("/configs/" + target_name + "/");

// Run the simulation
mcmc::execution::execute<IsingModelParameters>(
    mcmc::execution::ExpectationValueParameters::name(), target_name);
);
simulation.run();
```

If a running_parameter is defined in the generate_simulation() function, the simulation runs for different values of this parameter. In the example, the simulation is executed for 6 different values of beta that are equally distributed in the interval [0.1-0.7] (i.e., 0.1, 0.2, 0.3, 0.5, 0.6, 0.7).

### Implementation of the Ising Model

The library uses a the ParamHelper library to track all of the used parameter of the simluation. Therefore, the Ising Model is implemented by two classes. One class called IsingModelParameters which contains all parameters and one class called IsingModel that implements all functionalities. The two classes inherit from SystemBaseParameters and SystemBaseMeasure which define functions that need to be implemented for a specific theory.

class IsingModelParameters:
```c++
class IsingModelParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit IsingModelParameters(const json params):
            SystemBaseParameters(params),
            beta(get_entry<double>("beta", 0.4)),
            J(get_entry<double>("J", 1.0)),
            h(get_entry<double>("h", 0.0)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))

    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }
    }

    IsingModelParameters(double beta_, double J_, double h_, std::vector<int> dimensions_) :
        IsingModelParameters(json{
            {"beta", beta_},
            {"J", J_},
            {"h", h_},
            {"dimensions", dimensions_}
        })
    {}

    std::unique_ptr<IsingModel> generate() { return std::make_unique<IsingModel>(*this); }

private:
    friend class IsingModel;

    const double beta;
    const double J;
    const double h;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};
```

class IsingModel:
```c++
class IsingModel : public mcmc::simulation::SystemBase<IsingModel>
{
public:
    explicit IsingModel(const IsingModelParameters &imsp_) :
        imsp(imsp_),
        rand(std::uniform_real_distribution<double> (0,1)),
        rnd_lattice_site(std::uniform_int_distribution<uint>(0, get_size()))
    {}

    void update_step(uint measure_interval=1)
    {
        auto random_site_index = rnd_lattice_site(mcmc::util::gen);
        auto proposed_state = -1 * lattice[random_site_index];

        auto current_energy = local_energy(random_site_index, lattice[random_site_index]);
        auto proposal_energy = local_energy(random_site_index, proposed_state);

        if(rand(mcmc::util::gen) < std::min(1.0, std::exp(-1.0 * (proposal_energy - current_energy))))
            lattice[random_site_index] = proposed_state;
        // else
        //     reject
    }

    void initialize(std::string starting_mode)
    {
        generate_measures(imsp.measures);

        std::uniform_int_distribution<uint> uniint(0, 1);
        lattice = std::vector<int> (get_size(), 0);
        for(auto &site : lattice)
            site = 2 * uniint(mcmc::util::gen) - 1;
    }

    uint16_t get_size() const
    {
        return imsp.n_sites;
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

    double local_energy(uint site_index, int site_value)
    {
        double energy = 0;
        for(uint d = 0; d < imsp.dimensions.size(); d++)
        {
            energy += lattice[neigh_dir(site_index, d, true)];
            energy += lattice[neigh_dir(site_index, d, false)];
        }
        return  -1.0 * imsp.beta * site_value * (imsp.J * energy + imsp.h); // 0.5
    }

private:
    std::vector<int> lattice;

    const IsingModelParameters &imsp;

    std::uniform_real_distribution<double> rand;
    std::uniform_int_distribution<uint> rnd_lattice_site;

    //site, moving dimension, direction
    int neigh_dir(int n, int d, bool dir) const {
        if(dir)
            return n-n%(imsp.dim_mul[d]*imsp.dimensions[d]) +
                (n+imsp.dim_mul[d])%(imsp.dim_mul[d]*imsp.dimensions[d]);
        else
            return n-n%(imsp.dim_mul[d]*imsp.dimensions[d])+
                (n-imsp.dim_mul[d]+imsp.dim_mul[d]*imsp.dimensions[d]) %
(imsp.dim_mul[d]*imsp.dimensions[d]);
    }
};
```

The entire examples can be found in the examples/ folder. A thorough evaluation of the numerical results is implemented here: https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/jupyter_notebooks/ising_model_cheat_sheet.ipynb.


Projects using the MCMCSimulationLib library
-------------------------------------------------------

- LatticeModelImplementations (https://github.com/statphysandml/LatticeModelImplementations)


