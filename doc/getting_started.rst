Getting Started
===============

Prerequisites
-------------

Building MCMCSimulationLib requires the following software installed:

* A C++14-compliant compiler
* CMake `>= 3.15`
* Cookiecutter, e.g. by doing pip install cookiecutter, for auto-generating new projects
* Doxygen (optional, documentation building is skipped if missing) (not present, yet)
* Python `>= 3.6` for building Python bindings and for running the evaluation scripts of the library (will follow)

as well as the following python dependencies for a successful usage of the evaluation of numerical results:

* [MCMCEvaluationLib](https://github.com/statphysandml/MCMCEvaluationLib)
* [pystatsplottools](https://github.com/statphysandml/pystatplottools)

Installation
------------

The following sequence of commands builds MCMCSimulationLib. If you use a virtual envirnonment, it is important that it is activated for the building process to find the correct python version. The sequence assumes that your current working directory is the top-level directory
of the freshly cloned repository::

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    make install

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF} / {other}` to the `cmake` call:

* `CMAKE_INSTALL_PREFIX`: If the lilbrary is supposed to be installed locally, for example: /home/<user>/MCMCSimulationLib/install
* `BUILD_PYTHON_BINDINGS`: Enable building the Python bindings (default: `ON`)
* `RUN_WITH_PYTHON_BACKEND`: Use Python backend to compute expectation values, etc.
* `BUILD_TESTING`: Enable building of the test suite (default: `ON`)
* `BUILD_DOCS`: Enable building the documentation (default: `ON`)

The following additional CMake variables are only required if the library is used to submit jobs on a cluster and if a virtual environment is used for the python parts:

* `VIRTUAL_ENV`: Name of the virtual environment to be acivated before execution of the script
* `CONDA_ACTIVATE_PATH`: Path conda activate (for example: "~/miniconda3/bin/activate")

Build examples
--------------

Examples can be built by a similiar sequence of commands, where it is assumed that your current working directory is in the top-level folder of the respective example in MCMCSimulationLib/examples/::
    
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

The build process can be again customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF} / {other}` to the `cmake` call:

* `BUILD_PYTHON_BINDINGS`: Enable building the Python bindings (default: `ON`)
* `RUN_WITH_PYTHON_BACKEND`: Use Python backend to compute expectation values, etc.
* `CMAKE_PREFIX_PATH`: If the library is installed locally, for example: /home/<user>/MCMCSimulationLib/install

The additional CMake variable `CLUSTER_MODE` is only required if the library is used to submit jobs on a cluster or if this tested locally (for more information see ...):

* `CLUSTER_MODE`: Mode to run the cluster (default: "local")

If custom functions are used for evaluating observables or for loading the data, the CMake variable `PYTHON_SCRIPTS_PATH` defines the path to the resepctive python modules. The path is added
by C++ to the sys.path (for more information, see ...):

* `PYTHON_SCRIPTS_PATH`: Path to the potential python scripts evaluated by the python backend (default: "./python_scripts")


Generate your first application
-------------------------------

...and run