#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

#include <mcmc/mcmc_simulation/util/tqdm.h>
#include <param_helper/params.hpp>
#include <nlohmann/json.hpp>


using json = nlohmann::json;


namespace mcmc::simulation {
    class MarkovChain : public param_helper::params::Parameters {
    public:
        explicit MarkovChain(const json params) : Parameters(params)
        {
            measure_interval_ = get_entry<uint>("measure_interval");
            number_of_measurements_ = get_entry<uint>("number_of_measurements");
            repetitions_ = get_entry<uint>("repetitions");
            start_measuring_ = get_entry<uint>("start_measuring");
            starting_mode_ = get_entry<std::string>("starting_mode", "hot");
        }

        MarkovChain(
                uint measure_interval=1,
                uint number_of_measurements=1000,
                uint repetitions=1,
                uint start_measuring=0,
                std::string starting_mode = "hot"
        ) : MarkovChain(
                json{{"measure_interval",       measure_interval},
                        {"number_of_measurements", number_of_measurements},
                        {"repetitions",            repetitions},
                        {"start_measuring",        start_measuring},
                        {"starting_mode",          starting_mode}}) {}

        void write_to_file(const std::string &rel_root_dir) const {
            Parameters::write_to_file(rel_root_dir, "markovchain");
        }

        template<typename MS, typename SB>
        void run(MS &measurements, SB &systembase) const {
            unsigned long long int total_number_of_sweeps = long(repetitions_) * long(number_of_measurements_) * long(measure_interval_) + long(repetitions_) * long(start_measuring_);
            std::cout << "\n --- Running " << repetitions_ << " Markov Chain(s) to make " << number_of_measurements_
                        << " measurements with a measure interval of " <<
                        measure_interval_ << " and starting to measure after " << start_measuring_
                        << " sweeps ---\n ---- Total number of sweeps: "
                        << total_number_of_sweeps
                        << " ----" << std::endl;

            std::string starting_mode;
            if (starting_mode_ == "alternating")
                starting_mode = "hot";
            else
                starting_mode = starting_mode_;

            // Updating progress bar
            tqdm bar;
            bar.set_theme_line();

            for (uint z = 0; z < repetitions_; z++) {
                // Updating progress bar based on repetitions
                if(repetitions_ > 1)
                    bar.progress(z, repetitions_);
                
                // Initialize system
                // auto systembase = sbp.generate(); // Smart pointer
                systembase.init(starting_mode);
                
                // Equilibriate
                systembase.update(start_measuring_);

                // Initialize measurements
                measurements.initialize_measurements(starting_mode, systembase.measure_names(), z);
                systembase.init_measurements(starting_mode, z);

                // Reseting progress bar based on number of measurements
                if(repetitions_ == 1)
                    bar.reset();
                
                // Perform meauserments
                for (uint i = 0; i < number_of_measurements_; i++) {
                    // Updating progress bar based on number of measurements
                    if(repetitions_ == 1)
                        bar.progress(i, number_of_measurements_);
                    
                    // Measure
                    auto measures = systembase.measure();
                    measurements.process_measurements(measures);
                    
                    // Update for measure_interval steps
                    systembase.update(measure_interval_);
                }

                // Finalize measurements
                measurements.finalize_measurements();
                systembase.fina_measurements(starting_mode, z);

                // Finalizing progress bar based on number of measurements
                if(repetitions_ == 1)
                    bar.finish();

                if(starting_mode_ == "alternating") {
                    if (starting_mode == "hot")
                        starting_mode = "cold";
                    else
                        starting_mode = "hot";
                }
            }

            // Finalizing progress bar based on repetitions
            if(repetitions_ > 1)
                bar.finish();

            std::cout << " ---- Finished MCMC run(s) ----\n\n" << std::endl;
        }

    private:
        uint measure_interval_;
        uint number_of_measurements_;
        uint repetitions_;
        uint start_measuring_;
        std::string starting_mode_;
    };
}

#endif //MARKOV_CHAIN_HPP
