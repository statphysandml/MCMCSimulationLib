//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_CORRELATION_TIME_HPP
#define MAIN_CORRELATION_TIME_HPP

#include "param_helper/params.hpp"
#include "param_helper/json.hpp"

#include "../../mcmc_simulation/markov_chain.hpp"

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

    /* CorrelationTimeParameters(
            const std::string rel_config_path_
            uint minimum_sample_size_,
            uint start_measuring_,
            json measures_={}
    ) : CorrelationTimeParameters(
            json {{"minimum_sample_size", minimum_sample_size_},
                  {"maximum_correlation_time", maximum_correlation_time_},
                  {"start_measuring", start_measuring_},
                  {"measures", measures_}})
    {} */

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "correlation_time_params");
    }

    Parameters build_expanded_raw_parameters() const
    {
        Parameters parameters(params);
        return parameters;
    }

    static std::string name() {
        return "correlation_time";
    }

    std::unique_ptr<MarkovChainParameters> generate_markovchain_params(std::string running_parameter="None", double rp=0)
    {
        return std::make_unique<MarkovChainParameters>(
                1,
                minimum_sample_size + maximum_correlation_time,
                1,
                start_measuring,
                "hot");
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
