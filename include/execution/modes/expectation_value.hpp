//
// Created by lukas on 28.02.20.
//

#ifndef MAIN_EXPECTATION_VALUE_HPP
#define MAIN_EXPECTATION_VALUE_HPP

#include "param_helper/params.hpp"
#include "param_helper/json.hpp"

#include "../../mcmc_simulation/markov_chain.hpp"

using json = nlohmann::json;

class ExpectationValueParameters : public Parameters {
public:
    explicit ExpectationValueParameters(const json params_) : Parameters(params_)
    {
        measure_interval = get_value_by_key<uint>("measure_interval", 100);
        number_of_measurements = get_value_by_key<uint>("number_of_measurements", 1000);
        start_measuring = get_value_by_key<uint>("start_measuring", 0);

        measures = get_value_by_key<json>("measures", {});
        post_measures = get_value_by_key<json>("post_measures", {});
        n_means_bootstrap = get_value_by_key<uint>("n_means_bootstrap", 50);
    }

    ExpectationValueParameters(
            uint measure_interval_,
            uint number_of_measurements_,
            uint start_measuring_,
            json measures_={},
            json post_measures_={},
            uint n_means_bootstrap_=50
    ) : ExpectationValueParameters(
            json {{"measure_interval", measure_interval_},
                  {"number_of_measurements", number_of_measurements_},
                  {"start_measuring", start_measuring_},
                  {"measures", measures_},
                  {"post_measures", post_measures_},
                  {"n_means_bootstrap", n_means_bootstrap_}})
    {}

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "expectation_value_params");
    }

    Parameters build_expanded_raw_parameters() const
    {
        Parameters parameters(params);
        return parameters;
    }

    static std::string name() {
        return "expectation_value";
    }

    std::unique_ptr<MarkovChainParameters> generate_markovchain_params()
    {
        return std::make_unique<MarkovChainParameters>(
                measure_interval,
                number_of_measurements,
                1,
                start_measuring);
    }

    json& get_measures()
    {
        return measures;
    }

private:
    uint measure_interval;
    uint number_of_measurements;
    uint start_measuring;
    json measures;
    json post_measures;
    uint n_means_bootstrap;
};

#endif //MAIN_EXPECTATION_VALUE_HPP
