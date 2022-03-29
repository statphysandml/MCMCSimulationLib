Basic Features
==============

Markov Chain Monte Carlo systems
--------------------------------

MCMC system without the usage of predefined measures
****************************************************

.. code-block:: c++

   #ifndef MCMCSYSTEM_HPP
   #define MCMCSYSTEM_HPP


   #include <mcmc_simulation/header.hpp>


   // Template implementation for a MCMC system


   class MCMCSystem;


   struct MCMCSystemParameters : public mcmc::simulation::SystemBaseParameters {
      explicit MCMCSystemParameters(const json params):
               SystemBaseParameters(params),
               mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
               sigma(get_entry<double>("sigma", 0.4)),
               dt(get_entry<double>("dt", 0.01))
      {}

      MCMCSystemParameters(const std::vector<double> mu_, const double sigma_, const double dt_) : MCMCSystemParameters(json{
               {"mu", mu_},
               {"sigma", sigma_},
               {"dt", dt_}
      })
      {}

      std::unique_ptr<MCMCSystem> generate() { return std::make_unique<MCMCSystem>(*this); }

      const std::vector<double> mu;
      const double sigma;
      const double dt;
   };


   // Define which ones are optional...
   class MCMCSystem : public mcmc::simulation::SystemBase<MCMCSystem>
   {
   public:
      explicit MCMCSystem(const MCMCSystemParameters &sp_) :
         sp(sp_),
         normal(std::normal_distribution<double>(0.0, 1.0))
      {}

      void initialize(std::string starting_mode)
      {
         // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
         std::transform(sp.mu.begin(), sp.mu.end(), std::back_inserter(system), [this] (const double param) -> double { return this->normal(mcmc::util::gen); });
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
      void initialize_measurements(std::string starting_mode, uint rep=1)
      {}

      auto perform_measurements()
      {
         std::vector<std::variant<int, double, std::string>> results;
         for(const auto measure_name: get_measure_names())
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

      std::vector<std::string> get_measure_names()
      {
         return sp.get_measures();
      }

   private:
      std::vector<double> system; // Or any other system representation
      std::normal_distribution<double> normal;

      const MCMCSystemParameters &sp;
   };

   #endif //MCMCSYSTEM_HPP


MCMC system with the usage of predefined measures
*************************************************

.. code-block:: c++

   #ifndef MCMCMEASURESYSTEM_HPP
   #define MCMCMEASURESYSTEM_HPP


   #include <mcmc_simulation/header.hpp>


   // Template implementation for a MCMC measure system


   class MCMCMeasureSystem;


   struct MCMCMeasureSystemParameters : public mcmc::simulation::SystemBaseParameters {
      explicit MCMCMeasureSystemParameters(const json params):
               SystemBaseParameters(params),
               mu(get_entry<std::vector<double>>("mu", {0.0, 1.0})),
               sigma(get_entry<double>("sigma", 0.4)),
               dt(get_entry<double>("dt", 0.01))
      {}

      MCMCMeasureSystemParameters(const std::vector<double> mu_, const double sigma_, const double dt_) : MCMCMeasureSystemParameters(json{
               {"mu", mu_},
               {"sigma", sigma_},
               {"dt", dt_}
      })
      {}

      std::unique_ptr<MCMCMeasureSystem> generate() { return std::make_unique<MCMCMeasureSystem>(*this); }

      const std::vector<double> mu;
      const double sigma;
      const double dt;
   };
   class MCMCMeasureSystem : public mcmc::simulation::MeasureSystemBase<MCMCMeasureSystem>
   {
   public:
      explicit MCMCMeasureSystem(const MCMCMeasureSystemParameters &sp_) :
         sp(sp_),
         normal(std::normal_distribution<double>(0.0, 1.0))
      {}

      void initialize(std::string starting_mode)
      {
         // Called before every MCMC simulation for initalizing the system representation, starting mode can be "hot" or "cold", for example,
         std::transform(sp.mu.begin(), sp.mu.end(), std::back_inserter(system), [this] (const double param) -> double { return this->normal(mcmc::util::gen); });
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

      std::vector<std::string> get_measure_names()
      {
         return sp.get_measures();
      }

   private:
      std::vector<double> system; // Or any other system representation
      std::normal_distribution<double> normal;

      const MCMCMeasureSystemParameters &sp;
   };

   #endif //MCMCMEASURESYSTEM_HPP


Execution modes
---------------

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

Measurement processing
----------------------

Readable measures
*****************
.. doxygenstruct:: mcmc::measures::ReadableMeasureParameters

Simulation
----------

.. doxygenclass:: mcmc::simulation::SimulationParameters
    :members: SimulationParameters, generate_simulation, prepare_simulation_from_file, generate_simulation_from_file, write_to_file

.. doxygenclass:: mcmc::simulation::Simulation
    :members: Simulation, run, eval

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