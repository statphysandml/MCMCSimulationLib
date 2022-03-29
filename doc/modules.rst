Basic Features
==============

Simulation modes
----------------

EquilibriumTime
***************
.. doxygenclass:: mcmc::mode::EquilibriumTimeParameters
   :members: EquilibriumTimeParameters, write_to_file, evaluate

CorrelationTime
***************
.. doxygenclass:: mcmc::mode::CorrelationTimeParameters
   :members: CorrelationTimeParameters, write_to_file, evaluate

ExpectationValue
****************
.. doxygenclass:: mcmc::mode::ExpectationValueParameters
   :members: ExpectationValueParameters, write_to_file, evaluate

FromFilePreparation
*******************

Specific for running in ....

.. doxygenclass:: mcmc::mode::FromFilePreparation
   :members: FromFilePreparation, write_to_file, evaluate


Simulation
----------

Simulation
**********

.. doxygenclass:: mcmc::simulation::SimulationParameters
    :members: SimulationParameters, generate_simulation, prepare_simulation_from_file, generate_simulation_from_file, write_to_file

.. doxygenclass:: mcmc::simulation::Simulation
    :members: Simulation, run, eval

Sampler
-------

Gaussian sampler
****************
.. doxygenstruct:: mcmc::sampler::GaussianSampler

Uniform sampler
***************
.. doxygenstruct:: mcmc::sampler::UniformSampler

Hat function sampler
********************
.. doxygenstruct:: mcmc::sampler::HatFunctionSampler

Dummy sampler
*************
.. doxygenstruct:: mcmc::sampler::DummySampler

Measurement processing
----------------------

Readable measures
*****************
.. doxygenstruct:: mcmc::measures::ReadableMeasureParameters

Predefined measures
-------------------

Measure base class
******************

.. doxygenstruct:: mcmc::measures::Measure
    :members: measure, name

Mean
****
.. doxygenstruct:: mcmc::measures::Mean

Abs
***
.. doxygenstruct:: mcmc::measures::Abs

AbsMean
*******
.. doxygenstruct:: mcmc::measures::AbsMean

Second moment
*************
.. doxygenstruct:: mcmc::measures::SecondMoment


Fourth moment
*************
.. doxygenstruct:: mcmc::measures::FourthMoment

Variance
********
.. doxygenstruct:: mcmc::measures::Variance

Config
******
.. doxygenstruct:: mcmc::measures::Config

Util
----

Random number generation
************************

.. doxygenfile:: random.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::rd

.. doxygenvariable:: mcmc::util::gen

.. doxygenfunction:: mcmc::util::set_random_seed

Linspace intervals
******************

.. doxygenfunction:: mcmc::util::linspace

Sign function
*************

.. doxygenfunction:: mcmc::util::sign(const T value)


Python support
--------------

Python integration
******************

.. doxygenfile:: python_integration.hpp
   :sections: detaileddescription

.. doxygenvariable:: mcmc::util::is_python_initialized

.. doxygenfunction:: mcmc::util::initialize_python

.. doxygenfunction:: mcmc::util::finalize_python


Integrating external python modules
***********************************

.. doxygenvariable:: mcmc::util::python_modules_path

.. doxygenfunction:: mcmc::util::get_python_modules_path

.. doxygenfunction:: mcmc::util::set_python_modules_path


Advanced Features
=================


Command line support
--------------------

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