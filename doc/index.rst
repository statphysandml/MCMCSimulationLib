MCMCSimulationLib
=================

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


.. toctree::
   :maxdepth: 2
   :caption: Contents

   getting_started.rst
   basic_features.rst
   advanced_features.rst
   functional_features.rst
   python_modules.rst


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`