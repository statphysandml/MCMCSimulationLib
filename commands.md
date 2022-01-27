
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
cmake --build . --target mcmcsimulationlib_doxygen

cmake --build . --target mcmcsimulationlib-sphinx-doc

https://breathe.readthedocs.io/en/latest/class.html#class-example

# Useful link for a correct installation of your package

https://decovar.dev/blog/2021/03/08/cmake-cpp-library/

-> use: sudo make install


Alternatively

-DCMAKE_INSTALL_PREFIX=/home/lukas/install
-DCMAKE_PREFIX_PATH=/home/lukas/install

Basic Structure

- Further mention in each file which parameters need to be set and why!
- Replace basic example by phi^4 theory

C++ Simulation possibilities
- Based on Simulation class and Mode class -> Explains three main execution modes, Simulation class and basic evaluations -> /data and /results folder
- Mixing based on the different constructores of the simulation class
- Based on stored parameters / from_file -> /config folder -> Explains loading from file <-> Explain different constructors and point out their usages:
    - without mcmc::cmd <-> solely based on usage of the simulation::generate_from_file constructore (point out that this is also used in from_file and from_args)
    - mcmc::cmd::from_file: Allows local, on cpu and on gpu execution and preparation
    - mcmc::cmd::from_args: Allows local execution based on arguments passed in the terminal to the executable (used by from_file simulations), point out that from_args does the same as the local from_file simulation
- Based on default parameters (not that important at the moment)

Python Evaluation possibilites
- EvaluationModule class

C++ Simulation in Python based on python binding
- Bind your model to python (happens in the python_pybind/ directory of your project whereby all important C++ methods are wrapped up in the system class in "ising_model.py" and the c++ methods are binded in IsingModel_python.cpp using mcmc_pybind/pybind_simulation_parameters.hpp.)
- The wrapped class can then be utilized by ModeSimulation (of the MCMCSimulationLib/python_bind/ module) <-> run and evaluate the C++ model in the same manner as done in the C++ Simulations
- In addition the MCMCSimulation class of the MCMCEvaluationLib implements the same methods as the systembase class and implements the same execution modes as C++. This class can also be used with models written in Python!

Pure Python simulation
- Based on the MCMCSimulation and EvaluationModule class (are these fully independent of C++?)


Examples:

- Simulate and execute: Example for executing a simulation based on the Simulation class and Mode class - Without storing any parameters in config/
- Saving and loading: Example for generating and saving the simulation parmaters in the config directory and a subsequent loading and running of the simluation
- IsingModel: Full from file simulation that can also be run on cpu or gpu!