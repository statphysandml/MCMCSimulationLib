MCMCSimulationLib
=================

<!-- [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/statphysandml/MCMCSimulationLib/CI)](https://github.com/statphysandml/MCMCSimulationLib/actions?query=workflow%3ACI)
[![PyPI Release](https://img.shields.io/pypi/v/MCMCSimulationLib.svg)](https://pypi.org/project/MCMCSimulationLib)
[![Documentation Status](https://readthedocs.org/projects/MCMCSimulationLib/badge/)](https://MCMCSimulationLib.readthedocs.io/)
[![codecov](https://codecov.io/gh/statphysandml/MCMCSimulationLib/branch/main/graph/badge.svg)](https://codecov.io/gh/statphysandml/MCMCSimulationLib)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=statphysandml_MCMCSimulationLib&metric=alert_status)](https://sonarcloud.io/dashboard?id=statphysandml_MCMCSimulationLib) -->

MCMCSimulationLib is a C++ library that simplifies setting up Markov Chain Monte Carlo algorithms.
It implements the following basic features:

- Markov chain Monte Carlo simulations for arbitrary models/theories/systems in C++
- A modular structure allowing for a brute force and simple implementation of your MCMC system
- Computation of expectation values, autocorrelation times and the time the system needs to be in equilibrium
- Storing and tracking of simulation parameters and results based on configuration files and text files
- A sophisticated evaluation and processing of simulation results in Python, in particular, also with respect to a simple integration of the simulation outcomes in the Deep Learning framework PyTorch 

Further, it simplifies setting up and running simulations by providing support for:

- A Python wrapper for executing and evaluating simulations solely from Python
- A bash console/terminal interface allowing for running and preparing simulations based on configuration files
- Submitting and preparing simulations for an execution on a CPU/GPU cluster

In short, the library takes care of all the annoying parts of a Markov Chain Monte Carlo algorithm and leaves to you the implementation of the actual considered problem.

A detailed documentation of the MCMCSimulationLib can be found [here](https://mcmcsimulationlib.readthedocs.io/en/latest/).

Prerequisites
-------------

Building MCMCSimulationLib requires the following software installed:

* A C++17-compliant compiler
* CMake >= 3.15
* Python >= 3.6 for building Python bindings and for running the evaluation
  scripts of the library
* Cookiecutter, e.g. by doing ``pip install cookiecutter`` (needed for
  auto-generating new projects)
* Doxygen (optional, documentation building is skipped if missing)

as well as the following Python dependencies for supporting the evaluation and
processing of simulation results:

* [MCMCEvaluationLib](https://github.com/statphysandml/MCMCEvaluationLib)
* [pystatsplottools](https://github.com/statphysandml/pystatplottools)

Projects using the MCMCSimulationLib library
--------------------------------------------

- LatticeModelSimulationLib (https://github.com/statphysandml/LatticeModelSimulationLib)
- LatticeModelImplementations (https://github.com/statphysandml/LatticeModelImplementations)

Note that, currently, the two libraries use an older version of the MCMCSimulationLib. An update compatible with the newest version is in progress.

Support and Development
----------------------

The project was generated with the help of the [cookiecutter-cpp-project](https://github.com/ssciwr/cookiecutter-cpp-project) of the [Scientific Software Center, IWR, Heidelberg University](https://ssc.iwr.uni-heidelberg.de/).

Furthermore, the library uses [JSON for Modern C++](https://github.com/nlohmann/json) for reading and writing .json files and the [tqdm single header c++ pretty progress bar](https://github.com/aminnj/cpptqdm) by aminnj.

For bug reports/suggestions/complaints please file an issue on GitHub.

Or start a discussion on our mailing list: statphysandml@thphys.uni-heidelberg.de.
