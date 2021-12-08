#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

// #include <tiff.h>
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

            void write_to_file(const std::string &root_dir) {
                Parameters::write_to_file(root_dir, "markovchain_params");
            }

        private:
            template<typename>
            friend
            class MarkovChain;

            uint measure_interval;
            uint number_of_measurements;
            uint repetitions;
            uint start_measuring;
            std::string starting_mode;
        };

        template<typename SBP>
        class MarkovChain {
        public:
            explicit MarkovChain(const MarkovChainParameters &mp_, SBP &sbp_, std::ofstream &os_) : mp(mp_), sbp(sbp_),
                                                                                                    os(os_) {}

            void run() {
                std::cout << "Execute Markov Chain with " << mp.number_of_measurements
                          << " measures with measure interval " <<
                          mp.measure_interval << ".\nStart measuring after " << mp.start_measuring
                          << " sweeps.\n -- Total number of sweeps "
                          << long(mp.number_of_measurements) * long(mp.measure_interval) + long(mp.start_measuring)
                          << " -- " << std::endl;

                std::string starting_mode;
                if (mp.starting_mode == "alternating")
                    starting_mode = "hot";
                else
                    starting_mode = mp.starting_mode;

                for (uint z = 0; z < mp.repetitions; z++) {
                    //if(z%100 == 0)
                    //    std::cout << z*1.0/sim.rep << " " << std::flush;
                    auto systembase = sbp.generate(); // Smart pointer
                    systembase->init(starting_mode);
                    if (z == 0) {
                        std::vector<std::string> measure_names = systembase->measure_names();
                        if(measure_names.size() > 0)
                            os << measure_names[0];
                        for (uint i = 1; i < measure_names.size(); i++)
                            os << "\t" << measure_names[i];
                        if(measure_names.size() > 0)
                            os << std::endl;
                    }

                    systembase->update(mp.start_measuring);

                    for (uint i = 0; i < mp.number_of_measurements; i++) {
                        std::vector<std::string> measures = systembase->measure();
                        if(measures.size() > 0)
                            os << measures[0];
                        for (uint j = 1; j < measures.size(); j++)
                            os << "\t" << measures[j];
                        if(measures.size() > 0)
                            os << "\n";

                        if (i % 1000 == 0) {
                            std::cout << "########## i:" << i << /*"\n" << systembase <<*/ "\n";
                        }
                        systembase->update(mp.measure_interval);
                    }
                    std::cout << std::endl;

                    if (mp.starting_mode == "alternating") {
                        if (starting_mode == "hot")
                            starting_mode = "cold";
                        else
                            starting_mode = "hot";
                    }
                }
            }

        private:
            const MarkovChainParameters &mp;
            SBP &sbp;
            std::ofstream &os;
        };
    }
}

#endif //MARKOV_CHAIN_HPP
