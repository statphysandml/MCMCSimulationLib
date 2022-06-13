Basic Features
==============

The basic features of the MCMCSimulationLib are covered by the following modules
and allow to set up, to run and to evaluate a Markov chain Monte Carlo (MCMC)
simulation:

- :ref:`Markov chain Monte Carlo systems`: Two CRTP base classes,
  :cpp:class:`mcmc::simulation::SystemBase` and :cpp:class:`mcmc::simulation::MeasureSystemBase`, provide all necessary functions for running a MCMC simulation. The directory ``templates/`` contain templates which can be used to set up your own system.
- :ref:`Execution modes`: Modules to specify details of the simulation. The
  library features a computation of expectation values, of the autocorrelation time and of the time the system needs to be in equilibrium (to reach a steady state distribution). Results of previously executed simulations can be passed as an input to subsequent simulations.
- :ref:`Measurement processing`: Modules to handle the collection and 
  processing of measurements during the MCMC simulation. At the moment, there is only one class, :cpp:class:`mcmc::measures::ReadableMeasure`, which writes the simulation data in a readable format into a .txt file.
- :ref:`Simulation`: Core class of the MCMC simulation providing the 
  possibilities to run and evaluate simulations in different settings.
- :ref:`Predefined measures`: Standard measures on MCMC configurations, which
  can be used in combination with the :func:`mcmc::simulation::MeasureSystemBase`.

Note that all classes inherit from :cpp:class:`param_helper::params::Parameters`
providing the possibility to store all parameters of the respective class in
``.json`` files. This property allows for an automatic tracking of the
simulation parameters and enables to rerun the simulation at a later point.

Additionally, it is straightforward to bind the C++ modules into Python via
pybind11. This gives the possibility to execute the entire simulation solely
from Python.

Tutorials
---------

Setting up a simulation
***********************

The ``main.cpp`` program for running a Markov chain Monte Carlo simulation
consists of the following building blocks:

- Include important header files, define the relative path to the project root
  dir (in our case with respect to the build directory) and initialize Python if
  the CMake variable ``RUN_WITH_PYTHON_BACKEND`` has been set to ``ON``. The
  latter is required for evaluations of the MCMC simulation directly from C++
  code:

.. code-block:: c++

   #include "../include/mcmcsystem/config.h"

   #include <mcmc/mcmc_simulation/header.hpp>
   #include <mcmc/mcmc_simulation/util/intervals.hpp>
   #include <mcmc/modes/mode_header.hpp>
   #include <mcmc/mcmc_simulation/util/random.hpp>

   #include "../include/mcmcsystem/mcmcsystem.hpp"


   int main(int argc, char **argv) {
      // Initialize project dependent parameters
      param_helper::proj::set_relative_path_to_project_root_dir("../");

      #ifdef PYTHON_BACKEND
         mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
      #endif

- Define directories for storing the data and the results of the simulation, set
  up the MCMC system and initialize a measurement processor and an execution
  mode. In addition to the ``measures`` variable, there is the possibility to
  compute measures over the configurations by the ``post_measures`` variable,
  further details on that can be found in the :ref:`Custom measure and data
  loading functions` section:

.. code-block:: c++


   // Name of the simulation
   const std::string target_name = "MCMCSystemSimulation";

   // Directory for storing the results
   std::string rel_results_dir = "/results/" + target_name + "/";
   // Directory for storing the simulation data
   std::string rel_data_dir = "/data/" + target_name + "/";

   // Setting up the system
   MCMCSystem system({1.0, 2.0, -1.5}, 1.0, 0.002);

   // Setting up measurement processor
   typedef mcmc::measures::ReadableMeasure ReadableMeasureProcessor;
   ReadableMeasureProcessor readable_measures(rel_data_dir);

   // Setting up the execution mode
   typedef mcmc::mode::ExpectationValue ExpectationValueParams; ExpectationValueParams expectation_value_parameters(
      10, // measure_interval
      100000, //  number_of_measurements
      100, // start_measuring
      {"Config", "Mean"}, // measures
      {}, // post_measures
      "hot", // starting_mode
      "statistical" // error_type
   );

- Initialize the simulation class and use it to run and/or evaluate the MCMC
  simulation. The definition of a running parameter, specified by the variables
  ``running_parameter_kind`` and ``running_parameter`` provides the possibility
  to execute the same simulation for different values of this parameter, defined
  by the vector ``rp_intervals``. If these variables are omitted, the simulation
  only runs for the parameters defined when the system was initialized:

.. code-block:: c++

   // Prepare the simulation
   auto sigma_intervals = mcmc::util::linspace(0.2, 1.0, 9);
   auto expectation_value_simulation = mcmc::simulation::Simulation<
         MCMCSystem, ExpectationValueParams,
         ReadableMeasureProcessor>::generate_simulation(
            system,
            expectation_value_parameters,
            readable_measures,
            "systembase_params", // running_parameter_kind
            "sigma", // running parameter (rp)
            sigma_intervals // rp_intervals
   );
      
   // Run and evaluate the simulation expectation_value_simulation.run();
   expectation_value_simulation.eval(rel_results_dir);

- Finalize the program:

.. code-block:: c++

      // Finalization
   #ifdef PYTHON_BACKEND
      mcmc::util::finalize_python();
   #endif
      return 0;
   }

A full example with a simulation and computation of the time the system needs to
be in equilibrium, of the autocorrelation time and of expectation values can be
found in ``main.cpp`` of a project generated with the
``generate_application.py`` script, with ``standard`` as ``-mt`` parameter and
in the example code of the scalar theory.

Furthermore, note that each class implements a ``write_to_file(const std::string
rel_root_dir)`` function, which outputs .json file in the provided directory.
The simulation class provides several overloaded static constructors providing
the possibility to load simulations partially from configuration parameters
stored in these .json files. This feature is heavily used by the advanced
features of the library allowing for a simulation solely from the command line.

Writing your first Markov chain Monte Carlo system
**************************************************

The two files ``system.hpp`` and ``measures_system.hpp`` in the ``templates/``
directory can be used as a starting point to implement your own MCMC system. The
declaration of the classes can also be found :ref:`here<MCMC system without the
usage of predefined measures>` and :ref:`here<MCMC system with the usage of
predefined measures>`.

Note that these templates are also used by the ``generate_application.py``
script.

Evaluating results in Python
****************************

Evaluation module and PyTorch support
"""""""""""""""""""""""""""""""""""""

MCMCEvaluationLib provides support for loading and computing/recomputing
measures and simulation data generated with the help of the MCMCSimulationLib.
Possible evaluation tools are summarized in the ``EvaluationModule`` class.
Examples for using this class can be found in the ``python_examples/evaluation.py`` file of generated projects and of the scalar theory example.

Furthermore, examples in the ``pytorch_support/`` of these projects demonstrate
how the sampled MCMC configurations can be processed in the Deep Learning
framework PyTorch.

.. _Custom measure and data loading functions:

Custom measure and data loading functions
"""""""""""""""""""""""""""""""""""""""""

There is the possibility to define custom functions for loading data from file
and for defining custom measure functions. The former is useful if your data is
stored in a different format than the support ones. The latter can be helpful if
you want to compute expectation values for a measure that have not been tracked
by the C++ implementation (based on the ``measure`` parameters).

The support for this functionality is implemented in the
``mcmctools.loading.custom_function_support.py`` file of the MCMCEvaluationLib.
In particular, it looks for a module named ``custom_modules.py`` with the
declaration of the following two functions:

.. code-block:: python

   """ Input:
      data: pandas dataframe containing the MCMC configurations
      measure_name: Name of the measure
      custom_measures_args: Optional additional arguments; when called from C++ this refers to the serialized simulation parameters
   """
   def compute_measures(data, measure_name, custom_measures_args):
      import json
      sim_params = json.loads(custom_measures_args)
      pass

   """ Input:
      rel_data_dir: rel_data_dir to sim_base_dir
      running_parameter: running_parameter
      identifier: string indicating the type of performed execution mode ("equilibrium_time", "correlationtime" or "expectation_value"), sim_base_dir: when called from C++, this refers to the project_root_dir
      rp_values: list of values for the running_parameter
      custom_load_data_args: Optional additional arguments; when called from C++ this refers to the serialized simulation parameters
   """
   def custom_load_data(rel_data_dir, identifier, running_parameter,
   rp_values, sim_base_dir, custom_load_data_args):
      pass

For an evaluation based on the ``post_measures`` to work, one needs to store the
data which measures are supposed to be evaluated on, for example, the MCMC
configurations.

Additionally, the path to the ``custom_modules.py`` needs to be appended to the
Python ``sys.path``. When running from C++, this can be done by passing the
CMake variable ``PYTHON_SCRIPTS_PATH`` to cmake, for more details see also:
:ref:`Building the application`.

Examples can be found in the ``examples/`` directory or in the template
project generated with the ``generate_application.py`` script.

Integrating pybind11
********************

As indicated in the :ref:`Generate your first application` section, the library
supports an easy integration of your MCMC system into Python. The magic for this
happens in the ``python_pybind/`` directory of the generated project. The scalar
theory example also implements a respective binding of C++ functions and
contains example code for different use cases.

If the constructor of your MCMC system is different to the one of the generated
project, the ``.py`` and ``.cpp`` need to be adapted according to this changes.

Example code for using the python wrappers can be found in the
``python_examples/`` directory of the scalar theory example and of the template
project.

.. _Markov chain Monte Carlo systems:

Markov chain Monte Carlo systems
********************************

The following possible declarations of Markov chain Monte Carlo systems without and with the
usage of predefined measures serve as a good starting point for the implementation of your own MCMC system.

.. _MCMC system without the usage of predefined measures:

MCMC system without the usage of predefined measures
""""""""""""""""""""""""""""""""""""""""""""""""""""

.. literalinclude:: ../templates/system.hpp
   :language: c++

.. _MCMC system with the usage of predefined measures:

MCMC system with the usage of predefined measures
"""""""""""""""""""""""""""""""""""""""""""""""""

.. literalinclude:: ../templates/measure_system.hpp
   :language: c++

.. _Execution modes:

Execution modes
---------------

The library features the following execution modes.

EquilibriumTime
***************
.. doxygenclass:: mcmc::mode::EquilibriumTime
   :members: EquilibriumTime, write_to_file, evaluate

CorrelationTime
***************
.. doxygenclass:: mcmc::mode::CorrelationTime
   :members: CorrelationTime, write_to_file, evaluate

ExpectationValue
****************
.. doxygenclass:: mcmc::mode::ExpectationValue
   :members: ExpectationValue, write_to_file, evaluate

.. _Measurement processing:

Measurement processing
----------------------

So far, there is only one class to process measurements.

Readable measures
*****************

.. _ReadableMeasure:
.. doxygenstruct:: mcmc::measures::ReadableMeasure
   :members: ReadableMeasure, write_to_file

.. _Simulation:

Simulation
----------

The simulation class represents the main module for preparing, running and
evaluating simulations.

.. doxygenclass:: mcmc::simulation::Simulation
    :members: Simulation, generate_simulation, prepare_simulation_from_file, generate_simulation_from_file, write_to_file, run, eval

.. _Predefined measures:

Predefined measures
-------------------

List of predefined measures, which can be used in combination with the
:cpp:class:`mcmc::simulation::MeasureSystemBase` class.

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