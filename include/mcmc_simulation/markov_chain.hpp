#ifndef MARKOV_CHAIN_HPP
#define MARKOV_CHAIN_HPP

// #include <tiff.h>
#include "param_helper/params.hpp"
#include "param_helper/json.hpp"

#include "systembase.hpp"

using json = nlohmann::json;
//#include "systembase.hpp"

class MarkovChainParameters : public Parameters {
public:
    explicit MarkovChainParameters(const json params_) : Parameters(params_)
    {
        measure_interval = get_value_by_key<uint>("measure_interval");
        number_of_measurements = get_value_by_key<uint>("number_of_measurements");
        repetitions = get_value_by_key<uint>("repetitions");
        start_measuring = get_value_by_key<uint>("start_measuring");
        starting_mode = get_value_by_key<std::string>("starting_mode", "hot");
    }

    MarkovChainParameters(
            uint measure_interval_,
            uint number_of_measurements_,
            uint repetitions_,
            uint start_measuring_,
            std::string starting_mode_="hot"
    ) : MarkovChainParameters(
            json {{"measure_interval", measure_interval_},
                  {"number_of_measurements", number_of_measurements_},
                  {"repetitions", repetitions_},
                  {"start_measuring", start_measuring_},
                  {"starting_mode", starting_mode_}})
    {}

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "markovchain_params");
    }
private:
    template < typename >
    friend class MarkovChain;

    uint measure_interval;
    uint number_of_measurements;
    uint repetitions;
    uint start_measuring;
    std::string starting_mode;
};

template < typename SBP >
class MarkovChain {
public:
    explicit MarkovChain(const MarkovChainParameters &mp_, SBP &sbp_, std::ofstream &os_) : mp(mp_), sbp(sbp_), os(os_)
    {}

    void run()
    {
        std::cout << "Execute Markov Chain with " << mp.number_of_measurements << " measures with measure interval " <<
            mp.measure_interval << ".\nStart measuring after " << mp.start_measuring << " sweeps.\n -- Total number of total sweeps " << long(mp.number_of_measurements) * long(mp.measure_interval) + long(mp.start_measuring) << " -- " << std::endl;

        std::string starting_mode;
        if(mp.starting_mode == "alternating")
            starting_mode ="hot";
        else
            starting_mode = mp.starting_mode;

        for(uint z = 0; z < mp.repetitions; z++) {
            //if(z%100 == 0)
            //    std::cout << z*1.0/sim.rep << " " << std::flush;
            auto system_base = sbp.generate(); // Smart pointer
            system_base->initialize(starting_mode);
            if(z == 0) {
                std::vector<std::string> measure_names = system_base->measure_names();
                for(auto const& element: measure_names) {
                    os << element << "\t";
                }
                os << std::endl;
            }

            system_base->update(mp.start_measuring);

            for(uint i = 0; i < mp.number_of_measurements; i++) {
                std::vector<std::string> measures = system_base->measure();
                for(auto const& element: measures) {
                    os << element << "\t";
                }
                os << "\n";

                if ( i % 1000 == 0)
                {
                    std::cout << "########## i:" << i << /*"\n" << system_base <<*/ "\n";
                }
                system_base->update(mp.measure_interval);
            }
            std::cout << std::endl;

            if(starting_mode == "alternating")
            {
                if(starting_mode == "hot")
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


#endif //MARKOV_CHAIN_HPP
