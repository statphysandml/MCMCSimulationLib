#ifndef MCMCSIMULATIONLIB_FROM_FILE_PREPARATION_HPP
#define MCMCSIMULATIONLIB_FROM_FILE_PREPARATION_HPP

#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

#include "mcmc_simulation/markov_chain.hpp"
#include "mcmc_simulation/util/python_integration.hpp"

using json = nlohmann::json;

namespace mcmc {
    namespace mode {
        class FromFilePreparation : public param_helper::params::Parameters {
        public:
            explicit FromFilePreparation(const json params_) : Parameters(params_) {}

            FromFilePreparation() : FromFilePreparation(json{})
            {}

            void write_to_file(const std::string rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "from_file_preparation_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params);
                return parameters;
            }

            static std::string name() {
                return "from_file_preparation";
            }

            void evaluate(const std::string rel_data_dir, const std::string rel_results_dir,
                const std::string running_parameter="None", const std::vector<double>& rp_intervals=std::vector<double>{0.0}, const json simparams_json={})
            {}

            mcmc::simulation::MarkovChain
            generate_markov_chain(std::string running_parameter = "None", double rp = 0) {
                std::cerr << "-- A markov chain based on the from file preparation mode shouldn't be executed -- " << std::endl;
                return mcmc::simulation::MarkovChain(0, 0, 0, 0, "hot");
            }

            std::vector<std::string> get_measures() {
                return std::vector<std::string>{};
            }
        };

    }
}

#endif //MCMCSIMULATIONLIB_FROM_FILE_PREPARATION_HPP
