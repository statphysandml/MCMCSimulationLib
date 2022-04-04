MCMCSimulationLib
=================

<!-- [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/statphysandml/MCMCSimulationLib/CI)](https://github.com/statphysandml/MCMCSimulationLib/actions?query=workflow%3ACI)
[![PyPI Release](https://img.shields.io/pypi/v/MCMCSimulationLib.svg)](https://pypi.org/project/MCMCSimulationLib)
[![Documentation Status](https://readthedocs.org/projects/MCMCSimulationLib/badge/)](https://MCMCSimulationLib.readthedocs.io/)
[![codecov](https://codecov.io/gh/statphysandml/MCMCSimulationLib/branch/main/graph/badge.svg)](https://codecov.io/gh/statphysandml/MCMCSimulationLib)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=statphysandml_MCMCSimulationLib&metric=alert_status)](https://sonarcloud.io/dashboard?id=statphysandml_MCMCSimulationLib) -->

MCMCSimulationLib is a C++ library that simplifies setting up a Markov Chain Monte Carlo algorithms. The library provides a generic framework to perform simulations for standard tasks like a computation of expectation values or a determination of the autocorrelation time. Certain base classes enable an easy implementation of new theories and models.

An additional feature of the library is that all hyperparameters of a simulation can be saved and reused for the same simulation at a later point. Due to this, the library can be used to manage and start computations on a cluster pretty easy.

The computation of numerical results is implemented in Python. Besides an evaluation of the data, it is possible to load all configurations as a PyTorch dataset.

In short, the library takes care of all the annoying parts of a Markov Chain Monte Carlo algorithm and leaves to you the implementation of the actual considered problem.

More detailed instructions to get started can be found here: https://github.com/statphysandml/MCMCSimulationLib/blob/master/doc/getting_started.md. The guide should be read in parallel to the instructions, provided here, and to the README.md files of the the linked github respositories.

Prerequisites
-------------

Building MCMCSimulationLib requires the following software installed:

* A C++17-compliant compiler
* CMake `>= 3.15`
* Cookiecutter, e.g. by doing pip install cookiecutter, for auto-generating new projects
* Doxygen (optional, documentation building is skipped if missing) (not present, yet)
* Python `>= 3.6` for building Python bindings and for running the evaluation scripts of the library (will follow)

as well as the following python dependencies for a successful usage of the evaluation of numerical results:

* [MCMCEvaluationLib](https://github.com/statphysandml/MCMCEvaluationLib)
* [pystatsplottools](https://github.com/statphysandml/pystatplottools)


Building MCMCSimulationLib
--------------------------

The following sequence of commands builds MCMCSimulationLib. If you use a virtual envirnonment, it is important that it is activated for the building process to find the correct python version. The sequence assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF} / {other}` to the `cmake` call:

* `RUN_WITH_PYTHON_BACKEND`: Enable the computation of numerical results in Python (default: `ON`). Some features, like the computation of the correlation time do not work if this is disabled.
* `BUILD_TESTING`: Enable building of the test suite (default: `ON`)
* `BUILD_DOCS`: Enable building the documentation (default: `ON`)
<!-- * `BUILD_PYTHON_BINDINGS`: Enable building Python bindings (default: `ON`) -->

The following additional CMake variables are only required if the library is used to submit jobs on a cluster:

* `VIRTUAL_ENV`: Name of the virtual environment to be acivated before execution of the script
* `CONDA_ACTIVATE_PATH`: Path conda activate (for example: "~/miniconda3/bin/activate")

<!-- # Documentation

MCMCSimulationLib provides a Sphinx-based documentation, that can
be browsed [online at readthedocs.org](https://MCMCSimulationLib.readthedocs.io). -->

Examples
--------

Examples can be found in the examples/ directory. After a navigation into the top-level directory, the following sequence of commands builds the respective example. Note again that if you use a virtual envirnonment, it is important that it is activate for the building process to find the correct python version.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The build process can be again customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF} / {other}` to the `cmake` call:

* `CLUSTER_MODE`: Indicates whether the code is executed on the cluster (`on_cluster`) or locally (`local`) (default: `local`). The "local" option can be used to test whether the library prepares a computation on a cluster correctly. In this case, the simulation runs locally on your machine. The option can be changed to "on_cluster". In this case the jobs are sent to the cluster. There are two functions "prepare_execution_on_cpu_cluster" and "run_execution_on_cpu_cluster" that take care of this. The functions can be found in the file src/execution/execution.cpp and need to be adapted according to the used cluster. More details on how to execute a simulation on the cluster can be found in the main.cpp file of the SimulateAndExecute example (https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/SimulationAndExecution/src/main.cpp) or in the main.cpp file of a template project (see Template Project).
* `PYTHON_SCRIPTS_PATH`: Path to a directory including additional python files for a possible execution of code of custom written functions and modules. (default: `./python_scripts`). The path is appended by the programming to sys.path. It needs to be defined relative to the project root path.

As an example, in the project, a simulation of the Ising model is implemented. The Ising model is defined as a system classin examples/IsingModel/ising_model/include/system.hpp. The simulation can be executed by running ./IsingModel in the examples/build/. It is important that your virtual environment is activated before an execution! In the example, the correlation time is computed for several values of the inverse temperature. In the second part, simulations for the computation of expectation values is executed.

Configuration files to the different simulations are located in the examples/IsingModel/config directory. The simulation data is stored together with one single config file for the simulation in examples/data. Plots and numerical results for the correlation time as well as computed expectation values are saved in examples/results. The data evaluation takes place in Python based on code of the MCMCEvaluationLib and the pystatplottools library. A thorough evaluation of the simulation results is shown in a jupyter notebook that can be found here: https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/IsingModel/jupyter_notebooks/ising_model_cheat_sheet.ipynb. All important functionalities of the MCMCEvaluationLib are used for the data evaluation. Further plots are generated with the help of the pystatplottools library (https://github.com/statphysandml/pystatplottools).

Further examples in the examples/ directory contain code for a simple simulation and demonstrate how to store and load simulation parameters. Furthermore, it is explained in detail how a simulation can be executed within the library. In addition, the examples/IsingModel/python_scripts/examples directory shows several ways to use the MCMCEvaluationLib. The library also provides functionalities to load the generated Monte Carlo configurations as a PyTorch dataset. This makes the application of further machine learning tasks on the data pretty easy.

Template Project
----------------

A minimal project for an Ising model simulation can be generated as a starting point/template for your own MCMC simulation by running:

```python
python generate_application.py [-h] [-o OUTPUT_DIR] [--config-file CONFIG_FILE] [-n PROJECT_NAME]
```

where the arguments have the following meanings:

* -o, --output-dir: Where to output the generated project dir into.
* --config-file, User configuration file for overwriting default parameters of cookiecutter.json file in the application_wrapper/ directory (default: None)
* -n, --project_name: The name of the project (default: "my-mcmc-simulation-project")

Instruction for building the project can be found in a respective generated README.md file or in the previous examples section (which works equivalently).

The generated src/main.cpp file contains detailed instructions on different ways to execute the simulation.

Usage
-----

cmake (CMakeLists.txt) - The CMakeLists.txt files of the example projects and of the template project contain examples for a possible integration of the library with CMake.


A Simple Example - Simulation of an Ising Model
-----------------------------------------------

The following code samples explain basic usage of the MCMCSimulationLib. The mean value and other observables are computed for the Ising Model.

### Executing a Simulation

The code for the simulation is given by
```c++
// Setting up system parameters (beta, J, h, dimensions)
SystemParameters system_params(0.4, 1.0, 0.0, {4, 4});

/* Setting up execution parameters (number of equilibrium steps, number of measurements,
* correlation time, measures, optional measures that are computed in python) */
typedef mcmc::execution::ExpectationValue ExpectationValueParams;
ExpectationValueParams execution_parameters(
    100, 10000, 100, {"AbsMean", "SecondMoment", "Mean", "Config"}, {"Energy"}
);

// Setting up simulation parameters
std::string target_name = "IsingModelSimulation";
std::string rel_data_dir = "/data/" + target_name + "/";
auto simulation_params = mcmc::simulation::SimulationParameters<
    SystemParameters, ExpectationValueParams>::generate_simulation(
        system_params, execution_parameters, rel_data_dir, "systembase_params", "beta", 0.1, 0.7, 6
);

// Store simulation parameters
simulation_params.write_to_file("/configs/" + target_name + "/");

// Run the simulation
mcmc::execution::execute<SystemParameters>(
    mcmc::execution::ExpectationValue::name(), target_name);
);
simulation.run();
```

If a running_parameter is defined in the generate_simulation() function, the simulation runs for different values of this parameter. In the example, the simulation is executed for 6 different values of beta that are equally distributed in the interval [0.1-0.7] (i.e., 0.1, 0.2, 0.3, 0.5, 0.6, 0.7).

### Implementation of the Ising Model

The library uses a the ParamHelper library to track all of the used parameter of the simluation. Therefore, the Ising Model is implemented by two classes. One class called SystemParameters which contains all parameters and one class called System that implements all functionalities. The two classes inherit from SystemBaseParameters and SystemBaseMeasure which define functions that need to be implemented for a specific theory.

class SystemParameters:
```c++
class SystemParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit SystemParameters(const json params):
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

    SystemParameters(double beta_, double J_, double h_, std::vector<int> dimensions_) :
        SystemParameters(json{
            {"beta", beta_},
            {"J", J_},
            {"h", h_},
            {"dimensions", dimensions_}
        })
    {}

    std::unique_ptr<System> generate() { return std::make_unique<System>(*this); }

private:
    friend class System;

    const double beta;
    const double J;
    const double h;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};
```

class System:
```c++
class System : public mcmc::simulation::SystemBase<System>
{
public:
    explicit System(const SystemParameters &imsp_) :
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

    const SystemParameters &imsp;

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

The entire example can be found in the examples/IsingModel directory. A thorough evaluation of the numerical results is implemented here: https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/jupyter_notebooks/ising_model_cheat_sheet.ipynb.


Further comments
----------------

To keep track of changes in the submodules when pulling updates of the respository, we strongly recommend for an update on your local machine to use

```bash
git pull --recurse-submodules
```

otherwise the code might not run properly after an update of the library.


Projects using the MCMCSimulationLib library
-------------------------------------------------------

- LatticeModelSimulationLib (https://github.com/statphysandml/LatticeModelSimulationLib)
- LatticeModelImplementations (https://github.com/statphysandml/LatticeModelImplementations)


Support and Development
----------------------

The project was generated with the help of the [cookiecutter-cpp-project](https://github.com/ssciwr/cookiecutter-cpp-project) of the [Scientific Software Center, IWR, Heidelberg University](https://ssc.iwr.uni-heidelberg.de/).

For bug reports/suggestions/complaints please file an issue on GitHub.

Or start a discussion on our mailing list: statphysandml@thphys.uni-heidelberg.de


