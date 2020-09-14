//
// Created by lukas on 14.09.20.
//

#ifndef MCMCSIMULATIONLIB_EQUILIBRIATE_HPP
#define MCMCSIMULATIONLIB_EQUILIBRIATE_HPP

#include "param_helper/params.hpp"
#include "param_helper/json.hpp"

#include "../../mcmc_simulation/markov_chain.hpp"


// ToDo: You have to distinguish between hot and cold start somewhere, for example in the sim run file?
// or maybe add start_condition to mc parameters. It might also be reasonable to distinguish between appending data to the file and writing to the file

using json = nlohmann::json;

class EquilibriateParameters : public Parameters {
public:
    explicit EquilibriateParameters(const json params_) : Parameters(params_)
    {
        sample_size = get_value_by_key<uint>("sample_size", 100);
        number_of_steps = get_value_by_key<uint>("number_of_steps", 1000);
        measures = get_value_by_key<json>("measures", {});
        post_measures = get_value_by_key<json>("post_measures", {});
    }

    EquilibriateParameters(
            uint sample_size_,
            uint number_of_steps_,
            json measures_={},
            json post_measures_={}
    ) : EquilibriateParameters(
            json {{"sample_size_", sample_size_},
                  {"number_of_steps", number_of_steps_},
                  {"measures", measures_},
                  {"post_measures", post_measures_}})
    {}

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "equilibriate_params");
    }

    Parameters build_expanded_raw_parameters() const
    {
        Parameters parameters(params);
        return parameters;
    }

    static std::string name() {
        return "equilibriate";
    }

    std::unique_ptr<MarkovChainParameters> generate_markovchain_params(std::string running_parameter="None", double rp=0)
    {
        return std::make_unique<MarkovChainParameters>(
                1,
                number_of_steps,
                sample_size,
                0);
    }

    json& get_measures()
    {
        return measures;
    }

private:
    uint sample_size;
    uint number_of_steps;
    json measures;
    json post_measures;
};

#endif //MCMCSIMULATIONLIB_EQUILIBRIATE_HPP
