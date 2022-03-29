#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

#include "util/tqdm.h"
#include <param_helper/params.hpp>
#include <param_helper/json.hpp>


using json = nlohmann::json;


namespace mcmc {
    namespace simulation {

        class MarkovChainParameters : public param_helper::params::Parameters {
        public:
            explicit MarkovChainParameters(const json params_) : Parameters(params_) {
                measure_interval = get_entry<uint>("measure_interval");
                number_of_measurements = get_entry<uint>("number_of_measurements");
                repetitions = get_entry<uint>("repetitions");
                start_measuring = get_entry<uint>("start_measuring");
                starting_mode = get_entry<std::string>("starting_mode", "hot");
            }

            MarkovChainParameters(
                    uint measure_interval_,
                    uint number_of_measurements_,
                    uint repetitions_,
                    uint start_measuring_,
                    std::string starting_mode_ = "hot"
            ) : MarkovChainParameters(
                    json{{"measure_interval",       measure_interval_},
                         {"number_of_measurements", number_of_measurements_},
                         {"repetitions",            repetitions_},
                         {"start_measuring",        start_measuring_},
                         {"starting_mode",          starting_mode_}}) {}

            void write_to_file(const std::string &rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "markovchain_params");
            }

        private:
            template<typename>
            friend class MarkovChain;

            uint measure_interval;
            uint number_of_measurements;
            uint repetitions;
            uint start_measuring;
            std::string starting_mode;
        };

        template<typename SBP>
        class MarkovChain {
        public:
            explicit MarkovChain(const MarkovChainParameters &mp_, SBP &sbp_) : mp(mp_), sbp(sbp_)
            {}

            template<typename MS>
            void run(MS &measurements) {
                unsigned long long int total_number_of_sweeps = long(mp.repetitions) * long(mp.number_of_measurements) * long(mp.measure_interval) + long(mp.repetitions) * long(mp.start_measuring);
                std::cout << "\n --- Running " << mp.repetitions << " Markov Chain(s) to make " << mp.number_of_measurements
                          << " measurements with a measure interval of " <<
                          mp.measure_interval << " and starting to measure after " << mp.start_measuring
                          << " sweeps ---\n ---- Total number of sweeps: "
                          << total_number_of_sweeps
                          << " ----" << std::endl;

                std::string starting_mode;
                if (mp.starting_mode == "alternating")
                    starting_mode = "hot";
                else
                    starting_mode = mp.starting_mode;

                // Updating progress bar
                tqdm bar;
                bar.set_theme_line();

                for (uint z = 0; z < mp.repetitions; z++) {
                    // Updating progress bar based on repetitions
                    if(mp.repetitions > 1)
                        bar.progress(z, mp.repetitions);
                    
                    // Initialize system
                    auto systembase = sbp.generate(); // Smart pointer
                    systembase->init(starting_mode);
                    
                    // Equilibriate
                    systembase->update(mp.start_measuring);

                    // Initialize measurements
                    measurements.initialize_measurements(starting_mode, systembase->measure_names(), z);
                    systembase->init_measurements(starting_mode, z);

                    // Reseting progress bar based on number of measurements
                    if(mp.repetitions == 1)
                        bar.reset();
                    
                    // Perform meauserments
                    for (uint i = 0; i < mp.number_of_measurements; i++) {
                        // Updating progress bar based on number of measurements
                        if(mp.repetitions == 1)
                            bar.progress(i, mp.number_of_measurements);
                        
                        // Measure
                        auto measures = systembase->measure();
                        measurements.process_measurements(measures);
                        
                        // Update for measure_interval steps
                        systembase->update(mp.measure_interval);
                    }

                    // Finalize measurements
                    measurements.finalize_measurements();
                    systembase->fina_measurements(starting_mode, z);

                    // Finalizing progress bar based on number of measurements
                    if(mp.repetitions == 1)
                        bar.finish();

                    if (mp.starting_mode == "alternating") {
                        if (starting_mode == "hot")
                            starting_mode = "cold";
                        else
                            starting_mode = "hot";
                    }
                }

                // Finalizing progress bar based on repetitions
                if(mp.repetitions > 1)
                    bar.finish();

                std::cout << " ---- Finished MCMC run(s) ----\n\n" << std::endl;
            }

        private:
            const MarkovChainParameters &mp;
            SBP &sbp;
        };
    }
}

#endif //MARKOV_CHAIN_HPP
