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
               normal(std::normal_distribution<double>(0.0, 1.0))
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
         std::transform(mu.begin(), mu.end(), std::back_inserter(system), [this] (const double param) -> double { return this->normal(mcmc::util::gen); });
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


MCMC system with the usage of predefined measures
*************************************************

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
               normal(std::normal_distribution<double>(0.0, 1.0))
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

   private:
      std::vector<double> mu;
      double sigma;
      double dt;
      
      std::vector<double> system; // Or any other system representation
      std::normal_distribution<double> normal;
   };

   #endif //MCMCMEASURESYSTEM_HPP


Execution modes
---------------

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

Measurement processing
----------------------

Readable measures
*****************
.. doxygenstruct:: mcmc::measures::ReadableMeasure

Simulation
----------

.. doxygenclass:: mcmc::simulation::Simulation
    :members: Simulation, generate_simulation, prepare_simulation_from_file, generate_simulation_from_file, write_to_file, run, eval

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