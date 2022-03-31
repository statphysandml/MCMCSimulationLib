Getting Started
===============

Prerequisites
-------------

Building MCMCSimulationLib requires the following software installed:

* A C++17-compliant compiler
* CMake >= 3.15
* Python >= 3.6 for building Python bindings and for running the evaluation scripts of the library
* Cookiecutter, e.g. by doing "pip install cookiecutter" (needed for auto-generating new projects)
* Doxygen (optional, documentation building is skipped if missing)

as well as the following Python dependencies for supporting the evaluation and processing of simulation results:

* `pystatsplottools <https://github.com/statphysandml/pystatplottools>`_::

    git clone https://github.com/statphysandml/pystatplottools.git
    
    cd pystatplottools/
    
    python setup.py sdist
    pip install .

* `MCMCEvaluationLib <https://github.com/statphysandml/MCMCEvaluationLib>`_::

    git clone https://github.com/statphysandml/MCMCEvaluationLib.git

    cd MCMCEvaluationLib/
    
    python setup.py sdist
    pip install .

Installation
------------

After cloning the library with::

    git clone https://github.com/statphysandml/MCMCSimulationLib.git

the following sequence of commands builds MCMCSimulationLib. If you use a virtual envirnonment, it is important
that it is activated for the building process to find the correct python version. The sequence assumes that your
current working directory is the top-level directory
of the freshly cloned repository::

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ..
    cmake --build .
    make install

The build process can be customized with the following CMake variables,
which can be set by adding ``-D<var>={ON, OFF}/{other}`` to the ``cmake`` call:

* ``CMAKE_INSTALL_PREFIX``: If the lilbrary is supposed to be installed locally (for example: ../install)
* ``BUILD_PYTHON_BINDINGS``: Enable building Python bindings (default: ``ON``)
* ``RUN_WITH_PYTHON_BACKEND``: Use Python backend to compute expectation values, etc. (default: ``ON``)
* ``BUILD_TESTING``: Enable building of the test suite (default: ``ON``) (not fully implemented, yet)
* ``BUILD_DOCS``: Enable building the documentation (default: ``ON``)

The following additional CMake variables are only required if the library is used to submit jobs to a cluster:

* ``VIRTUAL_ENV``: Name of the virtual environment to be acivated before execution of the script
* ``CONDA_ACTIVATE_PATH``: Path conda activate (for example: ``~/miniconda3/bin/activate``)

for example::

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/MCMCSimulationLib/install -DVIRTUAL_ENV="rapunzel" -DCONDA_ACTIVATE_PATH="~/.miniconda3/bin/activate" ..

For more details, see ...

Using MCMCSimulationLib in another cmake application
****************************************************

After a successful building process, you can integrate the library into your C++ project by inserting the following line into your ``CMakeLists.txt`` file::

    find_package(MCMCSimulationLib CONFIG REQUIRED)

and by providing the ``CMAKE_PREFIX_PATH`` if the library was installed locally, i.e.::

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=~/MCMCSimulationLib/install ..

Python support
**************

The MCMCSimulationLib features the possiblity to easily write a wrapper for Python allowing for usage and exeuction of the C++ functionalities
solely from Python. Model-independent Python modules are summarized in the so-called `mcmcsimulationlib` package which can be built by the following
command where it is again assumed that your in the top-level folder of the respository::

    pip install --use-feature=in-tree-build .

For more details, see ...

Generate your first application
-------------------------------

A template project can be generated with the help of the ``generate_application.py`` script in the top-folder of the repository::

    python generate_application.py -h
    usage: Build Application [-h] [-o OUTPUT_DIR] [-n PROJECT_NAME] [-pdm PREDEFINED_MEASURES]
                             [-pyb PYTHON_BINDINGS] [-mt MAIN_TEMPLATE] [-cs CUDA_SUPPORT]

with the following parameters:

* ``OUTPUT_DIR``: Where to build the project.
* ``PROJECT_NAME``: The name of the project.
* ``PREDEFINED_MEASURES``: Build project with a predinfed support of standard measures: 'Yes' or 'No' (default).
* ``PYTHON_BINDINGS``: Whether building Python bindings should be support in the project: 'Yes' (default) or 'No'.
* ``MAIN_TEMPLATE``: Define the kind of template used as a starting point in the project: 'standard' (default), 'command_line_support' or 'cluster_support'.
* ``CUDA_SUPPORT``: Whether the exeuction of CUDA code should be supported: 'Yes' or 'No' (default).

i.e., for example::

    python generate_application.py -o "../Projects" -n "YourFirstProject"

The ``MAIN_TEMPLATE`` should be chosen in terms of the respective application. It aims to simplify using more advanced features of the library such as the possibility
to start simulations based on configuration files from the command line or to submit simulations to a cluster. Note that the only thing that changes when
choosing a different setting is how the ``main.cpp`` is structured. Additionally, the ``examples/`` directory contains examples for all the different
settings, allowing for a better exploration of the features of the libary.

Buidling the application
************************

After navigating to the project directory, the application can be built with a similar sequence of commands as above
for building one of the examples::
    
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=~/MCMCSimulationLib/install ..
    cmake --build .

The build process can be customized with the following CMake variables,
which can be set by adding ``-D<var>={ON, OFF}/{other}`` to the ``cmake`` call:

* ``CMAKE_PREFIX_PATH``: Required if the library is installed locally, for example: ``/home/<user>/MCMCSimulationLib/install``
* ``BUILD_PYTHON_BINDINGS``: Enable building Python bindings (default: ``ON``)
* ``RUN_WITH_PYTHON_BACKEND``: Use Python backend to compute expectation values, etc. (default: ``ON``)

The additional CMake variable ``CLUSTER_MODE`` is only required if the library is used to submit jobs on a cluster or if this tested locally (for more information see ...):

* ``CLUSTER_MODE``: Mode to run the cluster, ``local`` or ``on_cluster`` (default: ``local``). The ``local`` mode can be used to test the execution on a cluster first locally. By changing the parameter to ``on_cluster``, the code will be submitted and executed in the same manner on the cluster.

If custom functions are used for evaluating observables or for loading the data, the CMake variable ``PYTHON_SCRIPTS_PATH`` defines
the path to the resepctive python modules. The path is added in C++ to the ``sys.path`` (for more information, see ...):

* ``PYTHON_SCRIPTS_PATH``: Path to the potential python scripts evaluated by the python backend (default: ``./python_scripts``)

Executing the sample code
*************************

The example code can be executed aftwards by running::

    ./YourFirstProject

in the ``build/`` directory of your project. After the simulation, you should find a ``data/`` directory containing the numerical data
of the MCMC simualtion and a ``results/`` directory containing computed expectation values. To be able to properly relate the numerical data
with the utitlized simulation parameters, a .json file is automatically generated containing all these parameters.

Processing and accessing the results from Python
************************************************

The results and the MCMC simulation data can be viewed and loaded with the help of the MCMCEvaluationLib. Example code is provided in
the ``python_examples/evaluation.py`` file and the ``jupyter_notebooks/`` directory.

The latter can be started by navigating to the directory and running::

    cd ../jupyter_notebooks
    jupyter lab

Installing the python wrapper
*****************************

The support for python bindings of the generated project can be installed by running::

    pip install --use-feature=in-tree-build --install-option="--mcmcsimulationlib-cmake-prefix-path='~/MCMCSimulationLib/install/'" .

allowing for running simulations from Python. Note that it is only necessary to additionally provide the installation path of the
MCMCSimulation in case of a local installation.

Running the same simulation from Python
***************************************

If the installation of the python bindings was successful, exactly the same simulation can be executed from Python. Example code is provided in the
``python_examples/mode_simulation.py`` file, which can be executed by::

    cd python_examples
    python mode_simulation.py

For more details on the different functionalities of the library, see the tutorials section....

Build examples
--------------

The examples in the ``examples/`` directory can be built in the same manner as explained above. By showcasing different use cases of
the MCMCSimulationLib, they serve as a good starting point for exploring the functionalities of the library. In addition, the
tutorial section pick ups various code snippets to explain the features of the MCMCSimulationLib.