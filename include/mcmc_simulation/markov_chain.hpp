#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

#include "util/tqdm.h"
#include <param_helper/params.hpp>
#include <param_helper/json.hpp>


using json = nlohmann::json;


namespace mcmc {
    namespace simulation {
        class MarkovChain : public param_helper::params::Parameters {
        public:
            explicit MarkovChain(const json params_) : Parameters(params_)
            {
                measure_interval = get_entry<uint>("measure_interval");
                number_of_measurements = get_entry<uint>("number_of_measurements");
                repetitions = get_entry<uint>("repetitions");
                start_measuring = get_entry<uint>("start_measuring");
                starting_mode = get_entry<std::string>("starting_mode", "hot");
            }

            MarkovChain(
                    uint measure_interval_=1,
                    uint number_of_measurements_=1000,
                    uint repetitions_=1,
                    uint start_measuring_=0,
                    std::string starting_mode_ = "hot"
            ) : MarkovChain(
                    json{{"measure_interval",       measure_interval_},
                         {"number_of_measurements", number_of_measurements_},
                         {"repetitions",            repetitions_},
                         {"start_measuring",        start_measuring_},
                         {"starting_mode",          starting_mode_}}) {}

            void write_to_file(const std::string rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "markovchain_params");
            }

            template<typename MS, typename SB>
            void run(MS &measurements, SB &systembase) {
                unsigned long long int total_number_of_sweeps = long(repetitions) * long(number_of_measurements) * long(measure_interval) + long(repetitions) * long(start_measuring);
                std::cout << "\n --- Running " << repetitions << " Markov Chain(s) to make " << number_of_measurements
                          << " measurements with a measure interval of " <<
                          measure_interval << " and starting to measure after " << start_measuring
                          << " sweeps ---\n ---- Total number of sweeps: "
                          << total_number_of_sweeps
                          << " ----" << std::endl;

                std::string starting_mode_;
                if (starting_mode == "alternating")
                    starting_mode_ = "hot";
                else
                    starting_mode_ = starting_mode;

                // Updating progress bar
                tqdm bar;
                bar.set_theme_line();

                for (uint z = 0; z < repetitions; z++) {
                    // Updating progress bar based on repetitions
                    if(repetitions > 1)
                        bar.progress(z, repetitions);
                    
                    // Initialize system
                    // auto systembase = sbp.generate(); // Smart pointer
                    systembase.init(starting_mode_);
                    
                    // Equilibriate
                    systembase.update(start_measuring);

                    // Initialize measurements
                    measurements.initialize_measurements(starting_mode_, systembase.measure_names(), z);
                    systembase.init_measurements(starting_mode_, z);

                    // Reseting progress bar based on number of measurements
                    if(repetitions == 1)
                        bar.reset();
                    
                    // Perform meauserments
                    for (uint i = 0; i < number_of_measurements; i++) {
                        // Updating progress bar based on number of measurements
                        if(repetitions == 1)
                            bar.progress(i, number_of_measurements);
                        
                        // Measure
                        auto measures = systembase.measure();
                        measurements.process_measurements(measures);
                        
                        // Update for measure_interval steps
                        systembase.update(measure_interval);
                    }

                    // Finalize measurements
                    measurements.finalize_measurements();
                    systembase.fina_measurements(starting_mode_, z);

                    // Finalizing progress bar based on number of measurements
                    if(repetitions == 1)
                        bar.finish();

                    if (starting_mode == "alternating") {
                        if (starting_mode_ == "hot")
                            starting_mode_ = "cold";
                        else
                            starting_mode_ = "hot";
                    }
                }

                // Finalizing progress bar based on repetitions
                if(repetitions > 1)
                    bar.finish();

                std::cout << " ---- Finished MCMC run(s) ----\n\n" << std::endl;
            }

        private:
            uint measure_interval;
            uint number_of_measurements;
            uint repetitions;
            uint start_measuring;
            std::string starting_mode;
        };
    }
}

#endif //MARKOV_CHAIN_HPP
