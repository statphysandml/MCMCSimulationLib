MCMCSimulationLib
=================

MCMCSimulationLib is a C++ library that simplifies setting up a Markov Chain Monte Carlo algorithms. The library provides a framework to perform standard computations as a computation of expectation values or a determination of the autocorrelation time. A base class provides an interface for an easy implementation of new theories and models.

An additional feature of the library is that all simulations are automatically saved and can reused for the same simulation at a later point. Due to this, the library can be used to manage and start computations on a cluster pretty easy.

Certain python modules are also part of the library. The python modules allow a more convenient evaluation of the numerical results.

In short, the library takes care of all the annoying parts of a Markov Chain Monte Carlo algorithm and leaves you with the interesting task of the actual considered problem.

Build
-----

Certain configuration parameters need to be defined for an execution of the C++ program. They are defined in the "config.sh" file in the "build" directoy. The file config_template.sh contains a example for the definition of the parameters:
```bash
cluster_mode="local" # local/on_cluster

path_to_python3="~/.miniconda3/envs/virtual_env/" # (optional)
path_to_conda_activate="~/.miniconda3/bin/activate" # (optional)
virtual_env="virtual_env" # (optional)
python_version="3.7" # (optional)
```

The first parameter "cluster_mode" indicates whether the algorithms are started on the cluster (on_cluster) or locally (local).

#### cluster_mode = local

There is nothing that needs to be done. The "local" option can be used to test whether the library prepares a computation on a cluster correctly. In this case, the simulation runs locally on your machine.

#### cluster_mode = on_cluster

The option can be changed to "on_cluster". In this case the jobs are sent to the cluster. There are two functions "prepare_execution_on_cpu_cluster" and "run_execution_on_cpu_cluster" that take care of this. The functions can be found in the file src/execution/execution.cpp and need to be adapted in dependence on your cluster.

#### python parameters

The library can be used with or without the python modules. If one wants to make use of the python modules, the C++ program needs to know which virtual environment your are using. If you do not define any of the optional parameters (by deleting these lines). The library leaves out the functionalities that depend on python.

Having a config.sh file in the build directory, the library can be build with
```bash
cd build
bash build.sh
```

Examples
--------

The examples can be generated with
```bash
cd bash_script
bash build_examples.sh
```

Template Project
----------------

A new project that demonstrates the basic functionalities of the library based on a template class for a model or theory can be generated in an arbitrary directory with
```bash
cd build
bash build_project.sh
```
The template class can be adapted to your actual considered problem.

Usage
-----

cmake (CMakeLists.txt) - The CMakeLists.txt files in the example project and the template project contain examples for possible integration into CMakeLists.

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
    100, 10000, 100, {"AbsMean", "SecondMoment", "Mean"}, {}
);


// Setting up simulation parameters
std::string target_name = "IsingModelSimulation";
std::string rel_data_path = "/data/" + target_name + "/";
auto simulation_params = mcmc::simulation::SimulationParameters<
    IsingModelParameters, ExpectationValueParams>::generate_simulation(
        system_params, execution_parameters, rel_data_path, "systembase_params", "beta", 0.1, 0.7, 6
);


// Setting up and running the actual simulation
mcmc::simulation::Simulation< IsingModelParameters, ExpectationValueParams > simulation(
    simulation_params
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

The entire examples can be found in examples/ising_model.hpp.





