Advanced Features
=================

Tutorials
---------

Running simulations from file / Command line support
****************************************************

Based on the example of the Ising model (3/4)

C++
- prepare_simulation_from_file / generate_simulation_from_file
- command line interface

Running simulations on a CPU / GPU cluster
******************************************

Based on the example of the ON model (5-GPU) and again scalar theory (6-CPU)

Cmake
- Define config.h file

C++
- Use cmdi and mcmc cluster execute


Command line support
--------------------

FromFilePreparation
*******************

Specific for running in ....

.. doxygenclass:: mcmc::mode::FromFilePreparation
   :members: FromFilePreparation, write_to_file, evaluate

Path parameters
***************

.. doxygenstruct:: mcmc::cmdint::PathParameters
   :members: get_rel_config_dir, get_rel_data_dir, get_rel_results_dir, get_rel_cpu_bash_script_dir, get_rel_gpu_bash_script_dir

Command line interface
**********************

.. doxygenfunction:: mcmc::cmdint::prep_default_execution

.. doxygenfunction:: mcmc::cmdint::execute

.. doxygenstruct:: mcmc::cmdint::CmdIntSim


Cluster support
---------------

Cluster parameter initalization
*******************************
.. doxygenfunction:: mcmc::cluster::initialize_cluster_params

Executing cluster simulations
*****************************
.. doxygenfunction:: mcmc::cluster::execute

CPU cluster integration
***********************

.. doxygenfunction:: mcmc::cluster ::prepare_execution_on_cpu_cluster

.. doxygenfunction:: mcmc::cluster ::run_execution_on_cpu_cluster

GPU cluster integration
***********************

.. doxygenfunction:: mcmc::cluster ::prepare_execution_on_gpu_cluster

.. doxygenfunction:: mcmc::cluster ::run_execution_on_gpu_cluster

Virtual environment integration
*******************************

.. doxygenvariable:: mcmc::virtualenv::g_conda_activate_path

.. doxygenvariable:: mcmc::virtualenv::g_virtual_env

.. doxygenfunction:: mcmc::virtualenv::set_conda_activate_path

.. doxygenfunction:: mcmc::virtualenv::set_virtual_env