#ifndef READABLE_MEASURE_HPP
#define READABLE_MEASURE_HPP


#include <variant>


#include <param_helper/params.hpp>
#include <param_helper/json.hpp>

using json = nlohmann::json;


namespace mcmc {
    namespace measures {
        template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
        template<class... Ts> overload(Ts...) -> overload<Ts...>;

        /** @brief Class taking care of initalizing and writing measurements to file.
         */
        struct ReadableMeasure : public param_helper::params::Parameters
        {
            explicit ReadableMeasure(const json params) : Parameters(params),
                rel_data_dir_(get_entry<std::string>("rel_data_dir", "./data/"))
            {}

            /** @brief Prepare directory for writing simulation data to file.
             * 
             *  @param rel_data_dir Relative path to the project_root_dir (set by param_helper::proj::set_relative_path_to_project_root_dir("../")) for storing the MCMC simulation data
             */
            ReadableMeasure(std::string rel_data_dir="./data/") :
                ReadableMeasure(
                    json{{"rel_data_dir", rel_data_dir}}
                )
            {}

            /** @brief Write the readable measure parameters as readable_meausre_params.json into rel_root_dir
             *
             * @param rel_root_dir Relative path to the project_root_dir for storing configuration files
             * @returns None
             */
            void write_to_file(const std::string rel_root_dir) {
                Parameters::write_to_file(rel_root_dir, "readable_measure_params");
            }

            Parameters build_expanded_raw_parameters() const {
                Parameters parameters(params_);
                return parameters;
            }

            static std::string name() {
                return "readable_measure";
            }

            std::string get_rel_data_dir() const
            {
                // Create folder if not present
                if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_data_dir_))
                {
                    // std::cout << "Create data directory" << std::endl;
                    param_helper::fs::generate_directory_if_not_present(rel_data_dir_);
                }
                return rel_data_dir_;
            }

            const std::string get_setting_filename() const
            {
                return gen_filename();
            }

            void initialize_run(const std::string mode, const std::string running_parameter="None", const double rp_value=0)
            {
                mode_ = mode;
                running_parameter_ = running_parameter;
                rp_value_ = rp_value;

                // Prepare file os
                os_ptr_ = std::make_unique<param_helper::fs::Fileos>(param_helper::proj::project_root() + get_rel_data_dir() + "/" + gen_filename() + ".dat");
            }

            void initialize_measurements(const std::string starting_mode, const std::vector<std::string> measure_names, const uint rep)
            {
                auto &os = os_ptr_->get();
                // Write header of output file
                if (rep == 0) {
                    if(measure_names.size() > 0)
                        os << measure_names[0];
                    for (uint i = 1; i < measure_names.size(); i++)
                        os << "\t" << measure_names[i];
                    if(measure_names.size() > 0)
                        os << std::endl;
                }
            }

            template<typename T>
            void process_measurements(T &measurements)
            {
                std::vector<std::string> measures(measurements.size());
                std::transform(measurements.begin(), measurements.end(), measures.begin(), []
                    (std::variant<int, double, std::string>& val)
                    {
                        return std::visit(
                            overload{
                                [](const int &va)       { return std::to_string(va); },
                                [](const double &va)   { return std::to_string(va); },
                                [](const std::string &va)   { return va; }
                            }, val);
                    }
                );

                auto &os = os_ptr_->get();
                if(measures.size() > 0)
                    os << measures[0];
                for (uint j = 1; j < measures.size(); j++)
                    os << "\t" << measures[j];
                if(measures.size() > 0)
                    os << "\n";
            }

            void finalize_measurements()
            {
                auto &os = os_ptr_->get();
                os.flush();
            }

            const std::string gen_filename() const
            {
                if(running_parameter_ == "None")
                    return mode_;
                else
                    return mode_ + "_" + running_parameter_ + "=" + std::to_string(rp_value_);
            }

            typedef ReadableMeasure Measure;

            std::string rel_data_dir_;

            std::string mode_;
            std::string running_parameter_;
            double rp_value_;
            std::unique_ptr<param_helper::fs::Fileos> os_ptr_;
        };
    }
}

#endif // READABLE_MEASURE_HPP