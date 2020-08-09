//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_CORRELATION_TIME_HPP
#define MAIN_CORRELATION_TIME_HPP

#include "../simulation_lib/parameters.hpp"
#include "../simulation_lib/json.hpp"
using json = nlohmann::json;

class CorrelationTimeParameters : public Parameters {
public:
    explicit CorrelationTimeParameters(const json params_) : Parameters(params_)
    {
        minimum_sample_size = get_value_by_key<uint>("minimum_sample_size", 100);
        maximum_correlation_time = get_value_by_key<uint>("maximum_correlation_time", 1000);
        start_measuring = get_value_by_key<uint>("start_measuring", 0);
        measures = get_value_by_key<json>("measures", {});
    }

    CorrelationTimeParameters(
            uint minimum_sample_size_,
            uint maximum_correlation_time_,
            uint start_measuring_,
            json measures_={}
    ) : CorrelationTimeParameters(
            json {{"minimum_sample_size", minimum_sample_size_},
                  {"maximum_correlation_time", maximum_correlation_time_},
                  {"start_measuring", start_measuring_},
                  {"measures", measures_}})
    {}

    void write_to_file(const std::string& root_dir, const std::string& filename) {
        Parameters::write_to_file(root_dir, "correlation_time_params_" + filename);
    }

    static std::string name() {
        return "correlation_time";
    }

    std::unique_ptr<MarkovChainParameters> generate_markovchain_params()
    {
        return std::make_unique<MarkovChainParameters>(
                1,
                minimum_sample_size + maximum_correlation_time,
                1,
                start_measuring);
    }

    json& get_measures()
    {
        return measures;
    }

private:
    uint minimum_sample_size;
    uint maximum_correlation_time;
    uint start_measuring;
    json measures;
};

#endif //MAIN_CORRELATION_TIME_HPP
