
cmake

https://sciencesoftcode.wordpress.com/2019/10/10/modern-cmake/
https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1

# Classic CMake
cd pybind11
mkdir build
cd build
cmake ..
make install

# CMake 3.15+
cd pybind11
cmake -S . -B build
cmake --build build -j 2  # Build on 2 cores
cmake --install build


# Build documentation

(dependencies: 
conda install sphinx
conda install -c conda-forge breathe
pip install sphinx-rtd-theme
)

cmake --build . --target mcmcsimulationlib_doxygen

cmake --build . --target mcmcsimulationlib-sphinx-doc

https://shunsvineyard.info/2019/09/19/use-sphinx-for-python-documentation/
https://github.com/tox-dev/sphinx-autodoc-typehints

https://breathe.readthedocs.io/en/latest/class.html#class-example
https://breathe.readthedocs.io/en/latest/file.html#file-example
https://sphinx-rtd-tutorial.readthedocs.io/en/latest/build-the-docs.html
https://jalammar.github.io/illustrated-transformer/

# Next ToDo:

- Finalize appiction wrapper for measure_system, command line support, cluster support and cuda support (currently only pybind and scalar theory like exmaple is working)

- Include table of how examples are generated
- Recheck the python part (in particular python_examples and respective data loaders) in the application wrapper
- Clearify ParamHelper support
- Fix correct pybinding of .cu files...
- Fix cmake install prefix in setup.py...
pip install --use-feature=in-tree-build . --install-option='--mcmcsimulationlib-cmake-prefix-path=~/MCMCSimulationLib/install/' -v

- Credits to tqdm hinzufügen!!!

- Set Git to ON in CmakeLists.txt...

# Useful link for a correct installation of your package

https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

-> use: sudo make install

Alternatively

-DCMAKE_INSTALL_PREFIX=~/MCMCSimulationLib/install
-DCMAKE_PREFIX_PATH=~/MCMCSimulationLib/install


-> cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/MCMCSimulationLib/install -DVIRTUAL_ENV="rapunzel" -DCONDA_ACTIVATE_PATH="~/.miniconda3/bin/activate" ..

Basic Structure

- Further mention in each file which parameters need to be set and why! (-> in particular with respect to the initializations)
- Replace basic example by phi^4 theory

C++ Simulation possibilities
- Based on Simulation class and Mode class -> Explains three main execution modes, Simulation class and basic evaluations -> /data and /results folder (#1 Scalar Theory simulation) ((mcmc::SimulationParameter::generate_simulation)) <->
Note that data directory is not cleaned before any run, allowing for running several simulations at different times, for example, one for each parameters on a cpu (very helpful here)
- Based on stored parameters / from_file -> /config folder -> Explains loading from file <-> Explain different constructors and point out their usages
    - ### Storing ###
    - system_parameters.write_to_file(), execution_parameters.write_to_file() (#2 More generic simulation from file)
    - simulation_parameters.write_to_file(), execution_parameters.write_to_file() (#3 Loading and Saving example for simple simulation)
    - SimulationParameters::perpare_simulation_from_file (allows for an easy simulation with different running parameters, more naturally) (#4 Full simulation, used by Ising Model example) - Useful for running simulation at a different time, can also be used to run simulation with different running parameters at different times..as long as the number of samples doesn't change...
    - ### Loading ###
    - mcmc::SimulationParamaters::generate_simulation() with path_parameters allows for rerunning the same simulation with different parameters (note that the rel_data_path needs to be set) (#2 More generic simulation from file)
    - mcmc::SimulationParameters::generate_simulation_from_file() solely based on parameters (3# Loading and Saving example for simple simulation, #4 Full simulation, used by Ising Model example)

- Command line interface
    - mcmc::cmdint::CmdIntSim: Allows local execution based on arguments passed in the terminal to the executable (used by from_file simulations), point out that from_args does the same as the local from_file simulation

- GPU / CPU support
    - extend mcmc::cmdi by mcmc::cluster::execute: Allows local, on cpu and on gpu execution and preparation

- Based on default parameters (not that important at the moment)

Python Evaluation possibilites
- EvaluationModule class (evaluation.py) <-> Allows to recompute mode expectation values and do load all kinds of results

C++ Simulation in Python based on python binding (python_pybind/)
- Mode simulation (mode_simulation.py) - Features the possibility to execute the same simulations as above just from python (inherits from EvaluationModule class and extends this class by the possiblity to also execute the simulations in C++)
- Custom simulation (complete access on the mcmc model of C++ in python <-> can be used in combination with the MCMCSimulation class to also perform the same simulations with a full control in python)

Pure Python simulation
- Based on the MCMCSimulation and EvaluationModule class