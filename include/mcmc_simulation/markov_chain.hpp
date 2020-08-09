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
    }

    MarkovChainParameters(
            uint measure_interval_,
            uint number_of_measurements_,
            uint repetitions_,
            uint start_measuring_
    ) : MarkovChainParameters(
            json {{"measure_interval", measure_interval_},
                  {"number_of_measurements", number_of_measurements_},
                  {"repetitions", repetitions_},
                  {"start_measuring", start_measuring_}})
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
};

template < typename SBP >
class MarkovChain {
public:
    explicit MarkovChain(const MarkovChainParameters &mp_, SBP &sbp_, std::ofstream &os_) : mp(mp_), sbp(sbp_), os(os_)
    {}

    void run()
    {
        uint num = mp.number_of_measurements + mp.start_measuring;

        std::cout << "Number of sweeps: " << num * mp.number_of_measurements << std::endl;

        for(uint z = 0; z < mp.repetitions; z++) {
            //if(z%100 == 0)
            //    std::cout << z*1.0/sim.rep << " " << std::flush;
            auto system_base = sbp.generate(); // Smart pointer
            if(z == 0) {
                std::vector<std::string> measure_names = system_base->measure_names();
                for(auto const& element: measure_names) {
                    os << element << "\t";
                }
                os << std::endl;
            }
            for(uint i = 0; i < num; i++) {
                if (i >= mp.start_measuring) {
                    std::vector<std::string> measures = system_base->measure();
                    for(auto const& element: measures) {
                        os << element << "\t";
                    }
                    os << std::endl;
                }
                if ( i >= mp.start_measuring && (i - mp.start_measuring) % 1000 == 0)
                {
                    std::cout << "########## i:" << i << /*"\n" << system_base <<*/ std::endl;
                }
                system_base->update(mp.measure_interval);
            }
        }
    }
private:
    const MarkovChainParameters &mp;
    SBP &sbp;
    std::ofstream &os;
};


#endif //MARKOV_CHAIN_HPP
