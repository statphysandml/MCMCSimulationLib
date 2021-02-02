# Getting started

This file can be used as a step-by-step guide to get the library running and to run certain examples. Use in addition the README.md of the MCMCSimulationLib and of the other used repositories for additional information to the different steps.

## Setting up the virtual environment for Python

### Virtual environment

Use some kind of virtual environment, for example miniconda3 (https://docs.conda.io/en/latest/miniconda.html) to be able to generate a virtual environment. Useful commands can be found here: https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html

Set up an environment and install the following packages:

- matplotlib
- numpy
- pandas (version >= 1.1)
- scipy
- (pytorch)
- (jupyter lab)

The MCMCSimulationLib makes use of two additional libraries. You can follow the instructions here or the ones given on the respective github pages.

### pystatplottools

Clone pystatplottools from https://github.com/statphysandml/pystatplottools

```bash
git clone https://github.com/statphysandml/pystatplottools.git
```

Activate your virtual environment and install pystatplottools

```bash
source activate <virtualenv>

cd pystatplottools/
python setup.py sdist
pip install -e .
```

Optional:
- To test the library and to see what it can do, run the notebook cheat_sheet.ipynb in the examples/ directory of pystatplottools

```bash
cd examples/
jupyter lab
```

- You can also take a look at the other examples to get more insights into the different modules

### MCMCEvaluationLib

Clone MCMCEvaluationLib from https://github.com/statphysandml/MCMCEvaluationLib

```bash
git clone https://github.com/statphysandml/MCMCEvaluationLib.git
```

Install MCMCEvaluationlib in your virtual environment

```bash
cd MCMCEvaluationLib/
python setup.py sdist
pip install -e .
```

## Building the MCMCSimulationLib

Clone MCMCSimulationLib from https://github.com/statphysandml/MCMCSimulationLib

```bash
git clone https://github.com/statphysandml/MCMCSimulationLib.git
```

Preparation

```bash
cd MCMCSimulationLib/build/
cp config_template.sh ./config.sh
```

Change the config.sh file according to your setup. Replace the brackets and the python version by your setup.

path_to_python3="<path_to_conda>/envs/<virtualenv>/" # (optional)
virtual_env="<virtualenv>" # (optional)
python_version="3.7" # (optional)
path_to_conda_activate="<path_to_conda>/envs/<virtualenv>/" # (optional)

Build the library

```bash
bash build.sh
```

The building process generates a lib/ directory which contains the library. It also fetches the code and builds certain submodules.

To keep track of changes in the submodules when pulling updates of the respository, we strongly recommend to use

```bash
git pull --recurse-submodules
```

otherwise the code might not run properly after an update of the library.

## Examples

Build examples

```bash
cd ../bash_scripts/
bash build_examples.sh
```

Generates executables for the Ising model as an example in the examples/ directory and for two further examples in the examples/simulations/ directory. 

The SaveAndLoad example demonstrate how to save and load simulation parameters and the SimulateAndExecute examples contains code for a minimal example that shows all existing different ways to run an actual simulation. More details on this can be found in the main.cpp file. The code in SimulateAndExecute is the same that is generated if a new project is built and can be used as a template for an own custom implementation.

Run the Ising model simulation (remember to have your virtual environment activated)

```bash
cd ../examples/release/
./IsingModel
```

What happend?

The Ising model has been simulated for different values of the inverse temperature. First, simulations for the determination of the correlation time are running. The simulation results are evaluated and the correlation time is computed. The used parameter sets can be found in the configs/ directory and in the data/ directory next to the sampled values. The results/ directory contains generated figures that show the correlation time and the correlation_time_results.json file contains the different determined correlation times. After that, the actual MCMC simulation runs, again for the different inverse temperatures. The correlation times are loaded beforehand from the results/ directory. The resulting sampled values are again evaluated in Python and stored in the expectation_value_results.json file in the results/ directory.

The jupyter notebook ising_model_cheat_sheet.ipynb in the jupyter_notebooks/ directory shows a possible evaluation of the simulation results. It also demonstrates how the sampled configurations can be loaded into pytorch and visualized.

The examples/python_scripts/examples/ directory contains further code for examples that focus, in particular, on different ways to load the data into pytorch. There are also examples that demonstrate how the correlation times and the expectatoin values can be computed again directly in python (note that the same code is executed also by C++ if Python is supported).

The other examples in the simulations directory can be run the same manner.

## Compiling the code

The executable can be build again by running in the respective release/ or debug/ directory

```bash
make -j4
```

CMake can be executed again by (or with Debug in the debug/ directory)

```bash
cmake ../cmake/ -DCMAKE_BUILD_TYPE=Release
```
By running cmake again, it is also possible to change config parameters, for example, to change from running the code locally to a submission to a cluster. More details can be found in the main.cpp file of the SimulateAndExecute example and in the template project (see below).

Sometimes it might be necessary for the changes to apply to delete all generated files from cmake in the release/ and debug/ directory.

## Build your own project

Navigate to MCMCSimulationLib/bash_scripts/ and copy the project_config.sh file

```bash
cp project_config_template.sh project_config.sh
```

You can leave the defined parameters for the moment. More details on the meaning of the parameters is explained in the README.md file.

Next, build a new (template) project by running:

```bash
bash build_project.sh
```

Follow the instructions in the terminal...and navigate to your project.

You can run the code of the defined template project with the executable in the release/ directory. The code demonstrates, as the SimulateAndExecute example code, different ways to execute the code. In particular, it imitates how the code can be submitted to a cluster. Because we have set the cluster_mode to local in the project_config file. The code is executed locally. Further details can be found the main.cpp file.

You are now ready to implement your own model =). You can do this by inheriting from the class SystemBase systembase.hpp file or the class PlainSystemBase in the plain_systembase.hpp file in the include/mcmc_simulation/. In the case of the systembase class, you can make use of already implemented measures that are applied on the Monte Carlo configurations, like the mean value. You can also add new measures by overloading the generate_measures function. In the case of the PlainSystemBase you can integrate your on way of computing measures.

You can use the template class in systembase_template.hpp or in plain_systembase_template.hpp as a starting point for your own systembase implementation.

To see how a larger project can be built on top of the MCMCSimulationLib, you can take a look at the implementation of the LatticeModelSimulationLib (https://github.com/statphysandml/LatticeModelSimulationLib).

## Execution modes

In addition to the above, you might also be interested in the implementation of new execution modes. The execution modes are managed in the include/execution/ directory. Each of these modes can be identified with a particular type of simulation that is commonly performed in MCMC simulation. These are, for example, the computation of the autocorrelation time or the computation of expectation values. The interaction with the Python modules for an easier evaluation, i.e., the explicit call to certain functions, can be found in the executer.hpp file. Feel free to add new useful execution modes.

The executer takes also care of different ways to execute the code and contains the code that generates the qsub files for a submission to the cluster. You might need to adapt certain parameters or lines of code, here.

## The LatticeModelSimulationLib

Short instruction for using the LatticeModelSimulationLib are provided. Make sure to have successfully set up a virtual environment and to have installed the pystatplottools and the MCMCEvaluationLib. Instructions are given at the beginning of this file. It is not necessary to build the MCMCSimulationLib since this is done as part of the LatticeModelSimulationLib (The MCMCSimulationLib is integrated by a git submodule.).

Clone LatticeModelSimulationLib from https://github.com/statphysandml/LatticeModelSimulationLib

```bash
git clone https://github.com/statphysandml/LatticeModelSimulationLib.git
```

Follow the instructions on https://github.com/statphysandml/LatticeModelSimulationLib to build the library and to generate your first own project (works very similar to the respective steps to build the MCMCSimulationLib). Remember to provide a config.sh file in the build/ directory and a project_config.sh file in the bash_scripts/ directory.

In contrast to the MCMCSimulationLib, the template project of this library implements the O(n) Model. Instructions for different ways to execute the code are provided in the main.cpp file. After running the simulation, one can evaluate the simulation data with the help of the jupyter notebook in the jupyter_notebooks/ directory of the template project.

More examples, including the simulation of several models with different kinds of update rules are provided in the LatticeModelImplementations project (https://github.com/statphysandml/LatticeModelImplementations). Again, the building process is quite similar to the one of the MCMCSimulationLib and the LatticeModelSimulationLib.

For both projects, the used C++ libraries are integrated with submodules. To keep track of changes in the submodules it is again strongly recommended to also update the submodules when git pull is called,

```bash
git pull --recurse-submodules
```


## Advanced

### Custom measures

It is very likely that one wants to compute measures on the configurations which are not implemeted already. There is the possiblity to add code to do this either in Python or C++.

In C++, it is done by overloading the generate_measures function of SystemBase class. In the case of the systembase_template class, the measures need to inherit from the mcmc::common_measures::MeasurePolicy class. In the case of the plain_systembase_template class, one has the possibility to define measures in the perform_measure and the get_measure_names function.

In Python, one can generate a custom_measures.py file that implements a compute_measures function. For an example, see the Ising model implementation, i.e., the files in the examples/python_scripts/ directory. The path to this scripts needs to be set in C++. This can be done with the help of the python_modules_path in the project_config.sh file.

Custom measures for the pytorch dataset: For the generation of a pytorch dataset there exists a third possiblity to compute custom measures from the configurations. An example is given in the O(n) Model simulation of the LatticeModelImplementations project. The template project of the LatticeModelSimulationLib contains also an example where a measure is computed during the generation of a pytorch dataset.

### ... More topics will follow ...