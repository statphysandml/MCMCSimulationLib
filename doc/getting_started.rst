Getting Started
===============

Prerequisites
-------------

Building MCMCSimulationLib requires the following software installed:

* A C++17-compliant compiler (for g++>=9)
* CMake >= 3.15
* Python >= 3.6 for building Python bindings and for running the evaluation
  scripts of the library
* Cookiecutter, e.g. by doing ``pip install cookiecutter`` (needed for
  auto-generating new projects)
* Doxygen (optional, documentation building is skipped if missing)

as well as the following Python dependencies for supporting the evaluation and
processing of simulation results:

* `pystatsplottools <https://github.com/statphysandml/pystatplottools>`_, which
  can be downloaded and installed by::

    git clone https://github.com/statphysandml/pystatplottools.git
    
    cd pystatplottools/
    
    python setup.py sdist
    pip install .

* `MCMCEvaluationLib <https://github.com/statphysandml/MCMCEvaluationLib>`_,
  which can be downloaded and installed by::

    git clone https://github.com/statphysandml/MCMCEvaluationLib.git

    cd MCMCEvaluationLib/
    
    python setup.py sdist
    pip install .

.. _Installation:

Installation
------------

Building the library
********************

After cloning the library with::

    git clone https://github.com/statphysandml/MCMCSimulationLib.git

the following sequence of commands builds MCMCSimulationLib. If you use a
virtual environment, it is important that it is activated for the building
process to find the correct Python version. The sequence assumes that your
current working directory is the top-level directory of the freshly cloned
repository::

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ..
    cmake --build .
    make install

The build process can be customized with the following CMake variables, which
can be set by adding ``-D<var>={ON, OFF}/{other}`` to the ``cmake`` call:

* ``CMAKE_INSTALL_PREFIX``: If the library is supposed to be installed Device::locally
  (for example: ``../install``)
* ``BUILD_PYTHON_BINDINGS``: Enable building Python bindings (default: ``ON``)
* ``RUN_WITH_PYTHON_BACKEND``: Use Python backend to compute expectation values,
  etc. (default: ``ON``)
* ``BUILD_TESTING``: Enable building of the test suite (default: ``ON``) (not
  fully implemented, yet)
* ``BUILD_DOCS``: Enable building the documentation (default: ``ON``)

The following additional CMake variables are only required if the library is
used to submit jobs to a cluster:

* ``VIRTUAL_ENV``: Name of the virtual environment to be activated before
  execution of the script
* ``CONDA_ACTIVATE_PATH``: Path to conda activate (for example:
  ``~/miniconda3/bin/activate``)

for example::

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/MCMCSimulationLib/install -DVIRTUAL_ENV="rapunzel" -DCONDA_ACTIVATE_PATH="~/.miniconda3/bin/activate" ..

Using MCMCSimulationLib in another cmake application
****************************************************

After a successful building process, you can integrate the library into your C++
project by adding the following line to your ``CMakeLists.txt`` file::

    find_package(MCMCSimulationLib CONFIG REQUIRED)

and by providing the ``CMAKE_PREFIX_PATH`` if the library was installed Device::locally,
i.e.::

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=~/MCMCSimulationLib/install ..

Python support
**************

The MCMCSimulationLib features the possibility to easily set up a Python wrapper
to use and to execute C++ functionalities in Python.

MCMC system-independent Python modules are summarized in the so-called
`mcmcsimulationlib` package which can be built by the following command where it
is again assumed that your in the top-level folder of the repository::

    pip install --use-feature=in-tree-build .

Project-dependent modules need to be built separately. The application
generator, presented in the following, provides support for an easy integration
of your own MCMC system into Python.

.. _Generate your first application:

Generate your first application
-------------------------------

A template project can be generated with the help of the
``generate_application.py`` script in the top-folder of the repository::

    python generate_application.py -h
    usage: Build Application [-h] [-o OUTPUT_DIR] [-n PROJECT_NAME] [-pdm PREDEFINED_MEASURES]
                             [-pyb PYTHON_BINDINGS] [-mt MAIN_TEMPLATE]

with the following parameters:

* ``OUTPUT_DIR``: Where to build the project.
* ``PROJECT_NAME``: The name of the project.
* ``PREDEFINED_MEASURES``: Build project with a predefined support of standard
  measures: ``Yes`` or ``No`` (default).
* ``PYTHON_BINDINGS``: Whether building Python bindings should be support in the
  project: ``Yes`` (default) or ``No``.
* ``MAIN_TEMPLATE``: Define the kind of template used as a starting point in the
  project: ``standard`` (default), ``command_line_support`` or ``cluster_support``.

i.e., for example, by running::

    python generate_application.py -o "../Projects" -n "YourFirstProject"

The ``MAIN_TEMPLATE`` should be chosen in terms of the respective application.
It aims to simplify using more advanced features of the library such as the
possibility to start simulations based on configuration files from the command
line or to submit simulations to a cluster. Note that the only thing that
changes when choosing a different setting is how the ``main.cpp`` is structured.
Additionally, the ``examples/`` directory of the repository contains examples
for all the different settings, allowing for a better exploration of the
features of the library.

.. _Building the application:

Building the application
************************

After navigating to the project directory, the application can be built with a
similar sequence of commands as above::
    
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=~/MCMCSimulationLib/install ..
    cmake --build .

The build process can be customized with the following CMake variables, which
can be set by adding ``-D<var>={ON, OFF}/{other}`` to the ``cmake`` call:

* ``CMAKE_PREFIX_PATH``: Required if the library is installed Device::locally, for
  example: ``/home/<user>/MCMCSimulationLib/install``
* ``BUILD_PYTHON_BINDINGS``: Enable building Python bindings (default: ``ON``)
* ``RUN_WITH_PYTHON_BACKEND``: Use Python backend to compute expectation values,
  etc. (default: ``ON``)

The additional CMake variable ``CLUSTER_MODE`` is only required if the library
is used to submit jobs on a cluster or if running your job on a cluster is
tested Device::locally:

* ``CLUSTER_MODE``: Mode to run the cluster, ``local`` or ``on_cluster``
  (default: ``local``). The ``local`` mode can be used to test the execution on
  a cluster Device::locally. By changing the parameter to ``on_cluster``, the code will
  be submitted and executed in the same manner on the cluster.

More details can be found :ref:`here<Running simulations on a CPU / GPU
cluster>`.

If you want to add your own functions for evaluating observables or for loading
the MCMC simulation data, the CMake variable ``PYTHON_SCRIPTS_PATH`` can be used
to pass the path to the respective Python modules to the C++ application:

* ``PYTHON_SCRIPTS_PATH``: Path to a potential Python module integrated into the
  Python backend (default: ``./python_scripts``)

The path is automatically added to ``sys.path``. In addition, a file named
``custom_modules.py`` needs to be placed in the ``PYTHON_SCRIPTS_PATH`` and
serves as an interface to the MCMCEvaluationLib which looks for respective
methods implementing the customized behavior. More details can be found
:ref:`here<Custom measure and data loading functions>`.

Executing the sample code
*************************

The example code can be executed by running::

    ./YourFirstProject

in the ``build/`` directory of your project. After the simulation, you should
find a ``data/`` directory containing the numerical data of the MCMC simulation
and a ``results/`` directory containing plots and .json files with respect to
evaluated equilibrium times, correlation times and expectation values. To be
able to properly relate the numerical data with the utilized simulation
parameters, a .json file is automatically generated containing all configurable
parameters.

Processing and accessing the results from Python
************************************************

The results and the MCMC simulation data can be viewed and loaded with the help
of the MCMCEvaluationLib. Example code is provided in the
``python_examples/evaluation.py`` file.

Integration into PyTorch
************************

Examples for loading and integrating the MCMC simulation data into the Deep
Learning framework PyTorch can be found in the ``pytorch_support/`` directory.

Installing the Python wrapper
*****************************

The support for Python bindings of the generated project can be installed by
running::

    pip install --use-feature=in-tree-build --install-option="--mcmcsimulationlib-cmake-prefix-path='~/MCMCSimulationLib/install/'" .

allowing for running simulations from Python. Note that it is only necessary to
additionally provide the installation path of the MCMCSimulation in case of a
local installation.

Running the same simulation from Python
***************************************

An example for executing the same simulation as in the ``main.cpp`` file of the
generated project based on the Python wrapper is given in the
``python_examples/mode_simulation.py`` file, which can be executed by running::

    cd python_examples
    python mode_simulation.py


Examples for using further MCMC system related methods can be found in the
``python_examples/custom_simulation.py`` file. Additionally, the
``python_examples/mcmc_simulation.py`` serves as an example for a Python
simulation class making use of the Python wrapper to set up a MCMC simulation.

Build examples
--------------

Examples in the ``examples/`` directory can be built in the same manner as
explained above. By showcasing different use cases of the MCMCSimulationLib,
they serve as a good starting point for exploring the functionalities of the
library. In addition, the different tutorial sections picks up various code
snippets to explain the features of the MCMCSimulationLib.