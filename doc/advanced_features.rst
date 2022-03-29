Advanced Features
=================

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
   :members: get_rel_config_path, get_rel_data_path, get_rel_results_path, get_rel_cpu_bash_script_path, get_rel_gpu_bash_script_path

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

.. doxygenvariable:: mcmc::virtualenv::conda_activate_path

.. doxygenvariable:: mcmc::virtualenv::virtual_env

.. doxygenfunction:: mcmc::virtualenv::set_conda_activate_path

.. doxygenfunction:: mcmc::virtualenv::set_virtual_env