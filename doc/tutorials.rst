Tutorials
=========

Always include important code blocks!


Writing your first Markov chain Monte Carlo system
--------------------------------------------------

Running your first simulation
-----------------------------

Based on the example of scalar theory

Cmake
- Define CMake variables

Initialization
- Project_root_dir
- Path to python_scripts/Initialize Python

C++
- Run and evaluate the first simulation (1/2)
- In addition, introduce concept of configuration files - different arguments for generate_simulation

- Concept of rerunning with different rp_values

Python
- Evaluation module

Running simulations from file / Command line support
----------------------------------------------------

Based on the example of the Ising model (3/4)

C++
- prepare_simulation_from_file / generate_simulation_from_file
- command line interface

Running simulations on a CPU / GPU cluster
------------------------------------------

Based on the example of the ON model (5-GPU) and again scalar theory (6-CPU)

Cmake
- Define config.h file

C++
- Use cmdi and mcmc cluster execute


Integrating pybind11
--------------------

Again scalar theory

- Mode simulation
- Custom simulation

Doing Stuff in Python
- Pytorch
