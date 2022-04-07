Advanced Features
=================

Tutorials
---------

Running simulations from file / Command line support
****************************************************

The ``mcmc::cmdint::CmdIntSim<SB, MS>`` provides an interface for setting up and
executing simulations based on command line arguments to the executable.

The main function of the ``mcmc::cmdint::CmdIntSim`` struct:

.. code-block:: c++

   void main(int argc, char **argv)
   {
      if(argc > 1)
      {
         execute(argc, argv);
      }
      else
      {
         prepare();
      }
   }

calls in the case of additional command line arguments the member function
``execute`` , which prepares or runs a MCMC simulation based on the provided
template parameters systembase ``SB`` and measurement processor ``MS``.

Command line arguments need to be passed in the following order:

.. code-block::

   ./{executable} {execution_mode} {target_name} {sim_root_dir="./"} {rel_path=true} {run=true} {eval=true}

with the following variables:

- **{executable}** -- name of the executable
- **{execution_mode}** -- execution mode, one of ``equilibrium_time``,
  ``correlation_time`` and ``expectation_value``,
- **{target_name}** -- identifier of the current simulation
- **{sim_root_dir}** -- relative path to the ``project_root_dir`` (set by
  ``param_helper::proj::set_relative_path_to_project_root_dir("../")``) or
  absolute path to the simulation directory used for storing the
  configuration, data and result files of the simulation
- **{rel_path}** -- variable indicating whether ``sim_root_dir`` refers to an
  absolute or a relative path
- **{run}** -- variable indicating whether the MCMC simulation is performed
  (``true``) or not (``false``)
- **{eval}** -- variable indicated whether the evaluation should be performed in
  the same run (``true``) or not (``false``)

The virtual member function ``prepare`` can be overloaded to write all necessary
configuration files for a MCMC simulation to file. In both cases, the
``PathParameters`` class is used in the background to set up the necessary
directories in the filesystem.

The following code of a ``main.cpp`` file contains a minimal example for a
simulation with command line support:


.. code-block:: c++

   #include "../include/mcmcsystem/config.h"

   #include <mcmc_simulation/header.hpp>
   #include <mcmc_simulation/util/intervals.hpp>
   #include <modes/mode_header.hpp>
   #include <mcmc_simulation/util/random.hpp>

   #include <command_line_support/cmdint.hpp>

   #include "../include/mcmcsystem/mcmcsystem.hpp"


   struct ExpectationValueSimulation : mcmc::cmdint::CmdIntSim<MCMCSystem, mcmc::measures::ReadableMeasure>
   {
      using mcmc::cmdint::CmdIntSim<MCMCSystem, mcmc::measures::ReadableMeasure>::CmdIntSim;

      void prepare() override
      {
         auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);

         MCMCSystem system({1.0, 2.0, -1.5}, 1.0, 0.002);
         mcmc::measures::ReadableMeasure readable_measures(this->path_parameters_.get_rel_data_dir());

         auto simulation_parameters = mcmc::simulation::Simulation<MCMCSystem>::prepare_simulation_from_file(
               system, readable_measures,
               "systembase_params", "sigma", sigma_intervals);

         typedef mcmc::mode::ExpectationValue ExpectationValueParams;
         ExpectationValueParams expectation_value_parameters(
               10, 100000, 100, {"Config", "Mean"}, {}, "hot", "statistical");

         // Store simulation parameters
         simulation_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
         expectation_value_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
      }
   };


   int main(int argc, char **argv) {
      // Initialize project dependent parameters
      param_helper::proj::set_relative_path_to_project_root_dir("../");

   #ifdef PYTHON_BACKEND
      mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
   #endif

      // Setting up and calling the main function for command line support
      ExpectationValueSimulation expectation_value_simulation("MCMCSystemSimulation", "./", true);
      expectation_value_simulation.main(argc, argv);

      // Finalization
   #ifdef PYTHON_BACKEND
      mcmc::util::finalize_python();
   #endif
      return 0;
   }

Based on this setting, it is possible to, first, prepare a simulation by calling:

.. code-block::

   ./{executable}

and to execute the simulation based on the command described above, afterwards. 

The Ising model simulation in the ``examples/`` directory, as well as template
projects generated with the ``generate_application.py`` script and with
``command_line_support`` for the ``-mt`` parameter, represent additional
examples for simulations with command line support.

Note that for the preparation, the static constructor
``mcmc::simulation::Simulation<SB>::prepare_simulation_from_file`` is used in
combination with the particularly for this use case defined
``mcmc::mode::FromFilePreparation`` execution mode, corresponding to the default
``EP`` template argument of the ``mcmc::simulation::Simulation`` class.

Furthermore, note that if the ``prepare`` mode is not overloaded, running the
executable with command line arguments generates configuration files based on
the default constructors of the given MCMC system, measurement processor and
evaluation mode. In a next step, the configuration files can be modified.
Finally, by calling the same command again, the MCMC simulation will be
executed.

.. _Running simulations on a CPU / GPU cluster:

Running simulations on a CPU / GPU cluster
******************************************

The ``mcmc::cluster::execute`` function provides the possibility to prepare and
to submit jobs for executing simulations on a CPU or GPU cluster.

For this to work, it is important to adapt for running simulation on a cpu
cluster the ``mcmc::cluster::prepare_execution_on_cpu_cluster`` and
``mcmc::cluster::run_execution_on_cpu_cluster`` functions. Similar functions
exist for executions on a GPU cluster.

In the existing code, we make use of two additional global variables, namely
``g_executable_name`` and ``g_cluster_mode``. The two variables can be
initialized by

.. code-block:: c++

   mcmc::cluster::initialize_cluster_params(PROJECT_NAME, CLUSTER_MODE);

where ``PROJECT_NAME``, ``CLUSTER_MODE`` are defined in the provided examples in
a ``config.h`` file in the include directory which is generated by cmake.

By initializing the cluster parameters and by adding a function call of
``mcmc::cluster::execute`` to the example given above, the MCMC simulation can
be executed by simple running:

.. code-block::

   ./{executable}

where the script takes care of the following tasks:

- the ``prepare`` function generates the configuration files for the MCMC
  simulation and calls ``mcmc::cluster::execute`` with
  ``running_mode=prep_and_exec``
- the ``mcmc::cluster::execute`` generates the required bash script for a
  submission to the cluster and submits the job to cluster afterwards
- when the job is executed on the cluster, the executable is called with command
  line arguments and the ``execute`` function of the command line support struct
  is called which executes the actual MCMC simulation.

The modified ``main.cpp`` file looks as follows:

.. code-block:: c++

   #include "../include/mcmcsystem/config.h"

   #include <mcmc_simulation/header.hpp>
   #include <mcmc_simulation/util/intervals.hpp>
   #include <modes/mode_header.hpp>
   #include <mcmc_simulation/util/random.hpp>

   #include <command_line_support/cmdint.hpp>
   #include <cluster_support/cluster_integration.hpp>

   #include "../include/mcmcsystem/mcmcsystem.hpp"


   struct ExpectationValueSimulation : mcmc::cmdint::CmdIntSim<MCMCSystem, mcmc::measures::ReadableMeasure>
   {
      using mcmc::cmdint::CmdIntSim<MCMCSystem, mcmc::measures::ReadableMeasure>::CmdIntSim;

      void prepare() override
      {
         auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);

         MCMCSystem system({1.0, 2.0, -1.5}, 1.0, 0.002);
         mcmc::measures::ReadableMeasure readable_measures(this->path_parameters_.get_rel_data_dir());

         auto simulation_parameters = mcmc::simulation::Simulation<MCMCSystem>::prepare_simulation_from_file(
               system, readable_measures,
               "systembase_params", "sigma", sigma_intervals);

         typedef mcmc::mode::ExpectationValue ExpectationValueParams;
         ExpectationValueParams expectation_value_parameters(
               10, 100000, 100, {"Config", "Mean"}, {}, "hot", "statistical");

         // Store simulation parameters
         simulation_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());
         expectation_value_parameters.write_to_file(this->path_parameters_.get_rel_config_dir());

         // Prepare expectation value simulation on a cluster and submit the job with one function call
         mcmc::cluster::execute<MCMCSystem, mcmc::measures::ReadableMeasure>(
               "expectation_value", this->path_parameters_, true, true,
               mcmc::cluster::Device::on_cpu_cluster, mcmc::cluster::RunningMode::prep_and_exec, {});
      }
   };


   int main(int argc, char **argv) {
      // Initialize project dependent parameters
      param_helper::proj::set_relative_path_to_project_root_dir("../");

   #ifdef PYTHON_BACKEND
      mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
   #endif
      mcmc::cluster::initialize_cluster_params(PROJECT_NAME, CLUSTER_MODE);

      // Setting up and calling the main function for command line support
      ExpectationValueSimulation expectation_value_simulation("MCMCSystemSimulation", "./", true);
      expectation_value_simulation.main(argc, argv);

      // Finalization
   #ifdef PYTHON_BACKEND
      mcmc::util::finalize_python();
   #endif
      return 0;
   }

The ONModelGPU example serves as an additional example for a submission to a GPU
cluster. Note that a template project with a ``main.cpp`` script with cluster
support can also be generated by the ``generate_application.py`` script with
``cluster_support`` for the ``-mt`` parameter.

Command line support
--------------------

FromFilePreparation
*******************

.. doxygenclass:: mcmc::mode::FromFilePreparation
   :members: FromFilePreparation, write_to_file, evaluate

Path parameters
***************

.. doxygenstruct:: mcmc::cmdint::PathParameters
   :members: PathParameters, get_rel_config_dir, get_rel_data_dir, get_rel_results_dir, get_rel_cpu_bash_script_dir, get_rel_gpu_bash_script_dir

Command line interface
**********************

.. doxygenfunction:: mcmc::cmdint::prep_default_execution

.. doxygenfunction:: mcmc::cmdint::execute

.. doxygenstruct:: mcmc::cmdint::CmdIntSim
   :members: CmdIntSim, main, prepare


Cluster support
---------------

Cluster parameter initialization
********************************
.. doxygenfunction:: mcmc::cluster::initialize_cluster_params

Simulations on a cluster
************************
.. doxygenfunction:: mcmc::cluster::execute

CPU cluster integration
"""""""""""""""""""""""

.. doxygenfunction:: mcmc::cluster ::prepare_execution_on_cpu_cluster

.. doxygenfunction:: mcmc::cluster ::run_execution_on_cpu_cluster

GPU cluster integration
"""""""""""""""""""""""

.. doxygenfunction:: mcmc::cluster ::prepare_execution_on_gpu_cluster

.. doxygenfunction:: mcmc::cluster ::run_execution_on_gpu_cluster

Virtual environment integration
*******************************

The MCMCSimulationLib provides the possibility to pass the conda activate path
and the name of the used virtual environment to C++, since knowing these
variables can be necessary for generating the bash scripts for a submission to a
cluster:

.. doxygenvariable:: mcmc::virtualenv::g_conda_activate_path

.. doxygenvariable:: mcmc::virtualenv::g_virtual_env

As explained in the :ref:`Installation` section, the variables can be set
globally by providing respective CMake variables. Alternatively, the following
functions can be used to temporarily change these variables:

.. doxygenfunction:: mcmc::virtualenv::set_conda_activate_path

.. doxygenfunction:: mcmc::virtualenv::set_virtual_env