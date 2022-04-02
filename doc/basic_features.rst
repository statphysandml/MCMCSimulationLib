Basic Features
==============

The basic features of the MCMCSimulationLib are covered by the following modules and allow to set up,
to run and to evaluate a Markov chain Monte Carlo (MCMC) simulation:

- :ref:`Markov chain Monte Carlo systems`: Two CRTP base classes, :cpp:class:`mcmc::simulation::SystemBase` and :cpp:class:`mcmc::simulation::MeasureSystemBase`,
  provide all necessary functions for running a MCMC simulation. The directory ``templates/`` contains child classes which can be used to set
  up your own system.
- :ref:`Execution modes`: Modules to specify details of the simulation. The library features a computation of expectation values,
  of the autocorrelation time and of the time the system needs to be in equilibrium (to reach a steady state distribution).
  Results of previously executed simulations can be passed as an input to subsequent simulations.
- :ref:`Measurement processing`: Modules to handle the collection and processing of measurements during the MCMC simulation. At the moment, there is
  only one class, :cpp:class:`mcmc::measures::ReadableMeasure`, which writes the simulation data in a readable format into a .txt file.
- :ref:`Simulation`: Core class of the MCMC simulation providing the possibilities to run and evaluate a simulation in different settings.
- :ref:`Predefined measures`: Standard measures on MCMC configurations, which can be used in combination with the :func:`mcmc::simulation::MeasureSystemBase`.

Note that all classes inherit from :cpp:class:`param_helper::params::Parameters` providing the possibility to store all
parameters of the respective class in json files. This property allows for an automatic tracking of the simulation parameters
and enables to rerun the simulation at a later point.

Additionally, it is straightforward to bind the C++ modules into Python via pybind11.
This gives the possibility to execute the entire simulation solely from Python.

Tutorials
---------

Setting up a simulation
***********************

The ``main.cpp`` program for running a Markov chain Monte Carlo simulation consists of the following building blocks:

- Include important header files, define the relative path to the project root dir (in our case from the build directory) and initalize Python
  if the CMake variable ``RUN_WITH_PYTHON_BACKEND`` has been set to ``ON``. The latter is required for evaluations of the MCMC simulation
  directly from C++ code:

.. code-block:: c++

   #include "../include/mcmcsystem/config.h"

   #include <mcmc_simulation/header.hpp>
   #include <mcmc_simulation/util/intervals.hpp>
   #include <modes/mode_header.hpp>
   #include <mcmc_simulation/util/random.hpp>

   #include "../include/mcmcsystem/mcmcsystem.hpp"


   int main(int argc, char **argv) {
      // Initialize project dependent parameters
      param_helper::proj::set_relative_path_to_project_root_dir("../");

   #ifdef PYTHON_BACKEND
      mcmc::util::initialize_python(PYTHON_SCRIPTS_PATH);
   #endif

- Define directories for storing the data and the results of the simulation, set up the MCMC system and initialize a measurement processor
  and an execution mode. In addition to the ``measures`` variable, there is the possibility to compute measures over the configurations by
  the ``post_measures`` variable, further details on that can be found in the :ref:`Custom measure and data loading functions` section:

.. code-block:: c++

   // Name of the simulation
   const std::string target_name = "MCMCSystemSimulation";

   // Directory for storing the results
   std::string rel_results_path = "/results/" + target_name + "/";
   // Directory for storing the simulation data
   std::string rel_data_path = "/data/" + target_name + "/";

   // Setting up the system
   MCMCSystem system({1.0, 2.0, -1.5}, 1.0, 0.01);

   // Setting up measurement processor
   typedef mcmc::measures::ReadableMeasure ReadableMeasureProcessor;
   ReadableMeasureProcessor readable_measures(rel_data_path);

   // Setting up the execution mode
   typedef mcmc::mode::ExpectationValue ExpectationValueParams;
   ExpectationValueParams expectation_value_parameters(
      10, // measure_interval
      1000, //  number_of_measurements
      100, // start_measuring
      {"Config", "Mean"}, // measures
      {}, // post_measures
      "hot", // starting_mode
      "statistical" // error_type
   );

- Initialize the simulation class and use it to run and/or evaluate the MCMC simulation. The definition of a running parameter, specified by
  the variables ``running_parameter_kind`` and ``running_parameter`` provides the possibility to execute the same simulation for different values
  of this parameter, defined by the vector ``rp_intervals``. If these variables are omitted, the simulation only runs for the parameters defined
  when the system was initialized:

.. code-block:: c++

   // Prepare the simulation
   auto sigma_intervals = mcmc::util::linspace(0.5, 1.5, 9);
   auto expectation_value_simulation = mcmc::simulation::Simulation<
            MCMCSystem, ExpectationValueParams, ReadableMeasureProcessor>::generate_simulation(
            system,
            expectation_value_parameters,
            readable_measures,
            "systembase_params", // running_parameter_kind
            "sigma", // running parameter (rp)
            sigma_intervals // rp_intervals
   );
      
   // Run and evaluate the simulation
   expectation_value_simulation.run();
   expectation_value_simulation.eval(rel_results_path);

- Finalize the program:

.. code-block:: c++

   // Finalization
   #ifdef PYTHON_BACKEND
      mcmc::util::finalize_python();
   #endif
      return 0;
   }

A full example with a simulation of the scalar theory including the computation of the time the system needs to be in equilibrium,
of the autocorrelation time and of expectation values can be found here <link to scalar theory>.

Note that each class implements a ``write_to_file(const std::string rel_root_dir)`` function, which outputs .json file in the provided directory.
The simulation class provides several overloaded static constructors providing the possibility to load simulations partially from configuration
parameters stored in these .json files. This feature is heavily used by the advanced features of the library allowing for a simulation solely
from the command line.

Writing your first Markov chain Monte Carlo system
**************************************************

The two files ``system.hpp`` and ``measures_system.hpp`` in the ``templates/`` directory can be used as a starting point to implement your
own MCMC system. The declaration of the classes can also be found :ref:`here<MCMC system without the usage of predefined measures>` and
:ref:`here<MCMC system with the usage of predefined measures>`.

Note that these templates are also used by the ``generate_application.py`` script.

Evaluating results in Python
****************************

Evaluation module
"""""""""""""""""

MCMCEvaluationLib provides support for loading and computing/recomputing measures and simulation data generated with the help of
the MCMCSimulationLib. Possible evaluation tools are summarized in the ``EvaluationModule`` class. Examples for using this class can be found,
for example, in <link to examples>.

The examples also demonstrate how the MCMC configurations can be processed in the Deep Learning framework PyTorch.

.. _Custom measure and data loading functions:

Custom measure and data loading functions
"""""""""""""""""""""""""""""""""""""""""

There is the possibility to define custom functions for loading data from file and for defining custom measure functions. The former is useful
if your data is stored in a different format than the support ones. The latter can be helpful if you want to compute expectation values for
a measure that have not been tracked by the C++ implementation (based on the ``measure`` parameters).

The support for this functionality is implemented in the ``mcmctools.loading.custom_function_support.py`` file of the MCMCEvaluationLib.
In particular, it looks for a module named ``custom_modules.py`` with the declaration of the following two signatures:

.. code-block:: python

   """ Input:
      data: pandas dataframe containing the MCMC configurations
      measure_name: Name of the measure
      custom_measures_args: Optional additional arguments; when called from C++ this refers to the serialized simulation parameters
   """
   def compute_measures(data, measure_name, custom_measures_args):
      import json
      sim_params = json.loads(custom_measures_args)
      from scalar_theory_measures import compute_scalar_theory_measures
      return compute_scalar_theory_measures(data=data, measure_name=measure_name, sim_params=sim_params)

   """ Input:
      rel_data_dir: rel_data_dir to sim_base_dir
      running_parameter: running_parameter
      identifier: string indicating the type of performed exectution mode ("equilibrium_time",
         "correlationtime" or "expectation_value"),
      sim_base_dir: when called from C++, this refers to the project_root_dir
      rp_values: list of values for the running_parameter
      custom_load_data_args: Optional additional arguments; when called from C++ this refers
         to the serialized simulation parameters
   """
   def custom_load_data(rel_data_dir, running_parameter, identifier, sim_base_dir,
         rp_values, custom_load_data_args):
      pass

For an evaluation based on the ``post_measures`` to work, one needs to store the data the measures are supposed to be evaluated on,
for example the MCMC configurations.

Additionally, the path to the ``custom_modules.py`` needs to be appended to the Python ``sys.path``. When running from C++, this can be
done by passing the CMake variable ``PYTHON_SCRIPTS_PATH`` to cmake, for more details see also: :ref:`Building the application`.

Examples can be found in the ``examples/`` directory or in the template projected generated with ``generate_application.py`` file.

Integrating pybind11
********************

ToDo: In particular, one needs to adapt the files in python_pybind to the respective new constructor

<-> Reinclude code for both types of simulation

- Mode simulation
- Custom simulation

Doing Stuff in Python
- Pytorch

.. _Markov chain Monte Carlo systems:

Markov chain Monte Carlo systems
********************************

Possible declarations of Markov chain Monte Carlo systems without and with the usage of predefined measures.

.. _MCMC system without the usage of predefined measures:

MCMC system without the usage of predefined measures
""""""""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: c++

   #ifndef MCMCSYSTEM_HPP
   #define MCMCSYSTEM_HPP


   #include <mcmc_simulation/header.hpp>
   #include <mcmc_simulation/util/random.hpp>


   // Template implementation for a MCMC system


   class MCMCSystem : public mcmc::simulation::SystemBase<MCMCSystem>
   {
   public:
      explicit MCMCSystem(const json params):
               SystemBase(params),
               // Configuration parameters
               mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
               sigma(get_entry<double>("sigma", 0.4)),
               dt(get_entry<double>("dt", 0.01)),
               
               // Further member variables
               normal(std::normal_distribution<double>(0.0, 1.0)),
               system(std::vector<double>(mu.size(), 0.0))
      {}

      MCMCSystem(const std::vector<double> mu_={0.0, 1.0}, const double sigma_=0.4, const double dt_=0.01) : MCMCSystem(json{
               {"mu", mu_},
               {"sigma", sigma_},
               {"dt", dt_}
      })
      {}

      void initialize(std::string starting_mode)
      {
         // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
         if(starting_mode == "hot")
               std::transform(mu.begin(), mu.end(), system.begin(), [this] (const double param) -> double { return param + this->sigma * this->normal(mcmc::util::gen); });
         else
               std::fill(system.begin(), system.end(), 0);
      }

      void update_step(uint measure_interval=1)
      {
         // Called every MCMC step
         for(auto i = 0; i < get_size(); i++)
               system[i] = system[i] - dt * (system[i] - mu[i]) / std::pow(sigma, 2.0) + std::sqrt(2.0 * dt) * normal(mcmc::util::gen);
         
      }

      uint16_t get_size() const
      {
         // Returns the size of the system, for example,
         return system.size();
      }

      auto at(int i) const
      {
         // Read system state i of the system representation, for example,
         return system[i];
      }

      auto& at(int i)
      {
         // Return system state i of the system representation, for example,
         return system[i];
      }

      auto get_system_representation() const
      {
         // Returns the entire MCMC system representation, for example,
         return system;
      }

      auto& get_system_representation()
      {
         // Returns the entire MCMC system representation, for example,
         return system;
      }
      void initialize_measurements(std::string starting_mode, uint rep=1)
      {}

      auto perform_measurements()
      {
         std::vector<std::variant<int, double, std::string>> results;
         for(const auto measure_name: this->measure_names())
         {
               if(measure_name == "Mean")
               {
                  auto mean = std::accumulate(system.begin(), system.end(), 0.0);
                  results.push_back(mean / get_size());
               }
               else if(measure_name == "Config")
               {
                  std::string config = std::to_string(system[0]);
                  for (uint idx = 1; idx < get_size(); idx++)
                     config += ", " + std::to_string(system[idx]);
                  results.push_back(config);
               }
         }
         return results;
      }
      
      void finalize_measurements(std::string starting_mode, uint rep=1)
      {}
      
   private:
      std::vector<double> mu;
      double sigma;
      double dt;
      
      std::vector<double> system; // Or any other system representation
      std::normal_distribution<double> normal;
   };

   #endif //MCMCSYSTEM_HPP


.. _MCMC system with the usage of predefined measures:

MCMC system with the usage of predefined measures
"""""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: c++

   #ifndef MCMCMEASURESYSTEM_HPP
   #define MCMCMEASURESYSTEM_HPP


   #include <mcmc_simulation/header.hpp>
   #include <mcmc_simulation/util/random.hpp>


   // Template implementation for a MCMC measure system


   class MCMCMeasureSystem : public mcmc::simulation::MeasureSystemBase<MCMCMeasureSystem>
   {
   public:
      explicit MCMCMeasureSystem(const json params):
               MeasureSystemBase(params),
               // Configuration parameters
               mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
               sigma(get_entry<double>("sigma", 0.4)),
               dt(get_entry<double>("dt", 0.01)),
               
               // Further member variables
               normal(std::normal_distribution<double>(0.0, 1.0)),
               system(std::vector<double>(mu.size(), 0.0))
      {}

      MCMCMeasureSystem(const std::vector<double> mu_={0.0, 1.0}, const double sigma_=0.4, const double dt_=0.01) : MCMCMeasureSystem(json{
               {"mu", mu_},
               {"sigma", sigma_},
               {"dt", dt_}
      })
      {}

      void initialize(std::string starting_mode)
      {
         // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
         if(starting_mode == "hot")
               std::transform(mu.begin(), mu.end(), system.begin(), [this] (const double param) -> double { return param + this->sigma * this->normal(mcmc::util::gen); });
         else
               std::fill(system.begin(), system.end(), 0);
      }

      void update_step(uint measure_interval=1)
      {
         // Called every MCMC step
         for(auto i = 0; i < get_size(); i++)
               system[i] = system[i] - sp.dt * (system[i] - sp.mu[i]) / std::pow(sp.sigma, 2.0) + std::sqrt(2.0 * sp.dt) * normal(mcmc::util::gen);
         
      }

      uint16_t get_size() const
      {
         // Returns the size of the system, for example,
         return system.size();
      }

      auto at(int i) const
      {
         // Read system state i of the system representation, for example,
         return system[i];
      }

      auto& at(int i)
      {
         // Return system state i of the system representation, for example,
         return system[i];
      }

      auto get_system_representation() const
      {
         // Returns the entire MCMC system representation, for example,
         return system;
      }

      auto& get_system_representation()
      {
         // Returns the entire MCMC system representation, for example,
         return system;
      }

   private:
      std::vector<double> mu;
      double sigma;
      double dt;
      
      std::vector<double> system; // Or any other system representation
      std::normal_distribution<double> normal;
   };

   #endif //MCMCMEASURESYSTEM_HPP

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

.. _Simulation:

Simulation
----------

The simulation class represents the main module for preparing, running and evaluating simulations.

.. doxygenclass:: mcmc::simulation::Simulation
    :members: Simulation, generate_simulation, prepare_simulation_from_file, generate_simulation_from_file, write_to_file, run, eval

.. _Predefined measures:

Predefined measures
-------------------

List of predefined measures, which can be used in combination with a the :cpp:class:`mcmc::simulation::MeasureSystemBase`.

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