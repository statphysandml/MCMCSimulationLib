#ifndef READABLE_MEASURE_HPP
#define READABLE_MEASURE_HPP


#include <variant>


#include <param_helper/params.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace mcmc::measures {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class... Ts> overload(Ts...) -> overload<Ts...>;

    /** @brief Helper function for converting measurements to vector of
     * std::string
     */
    template<typename T>
    std::vector<std::string> measurements_to_string(T &measurements)
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
        return measures;
    }

    /** @brief Class taking care of initalizing and writing measurements to
     * file.
     */
    struct ReadableMeasure : public param_helper::params::Parameters
    {
        explicit ReadableMeasure(const json params);

        /** @brief Prepare directory for writing simulation data to file.
         *
         *  @param rel_data_dir Relative path to the ``project_root_dir``
         *  (set by
         *  ``param_helper::proj::set_relative_path_to_project_root_dir("../")``)
         *  for storing the MCMC simulation data
         */
        explicit ReadableMeasure(std::string rel_data_dir="./data/");

        /** @brief Write the readable measure parameters as
         * ``readable_measuse_params.json`` into ``rel_root_dir``.
         *
         * @param rel_root_dir Relative path to the ``project_root_dir`` for
         * storing configuration files
         * @returns None
         */
        void write_to_file(const std::string &rel_root_dir) const;

        param_helper::params::Parameters build_expanded_raw_parameters() const;

        static std::string name();

        std::string get_rel_data_dir() const;

        std::string get_setting_filename() const;

        void initialize_run(std::string_view mode, std::string_view running_parameter="None", const double rp_value=0);

        void initialize_measurements(std::string_view starting_mode, const std::vector<std::string> &measure_names, const uint rep) const;

        template<typename T>
        void process_measurements(T &measurements)
        {
            auto measures = measurements_to_string(measurements);

            auto &os = os_ptr_->get();
            if(measures.empty())
                os << measures[0];
            for (uint j = 1; j < measures.size(); j++)
                os << "\t" << measures[j];
            if(measures.empty())
                os << "\n";
        }

        void finalize_measurements() const;

        std::string gen_filename() const;

        using Measure = ReadableMeasure;

        std::string rel_data_dir_;

        std::string mode_;
        std::string running_parameter_;
        double rp_value_;
        std::unique_ptr<param_helper::fs::Fileos> os_ptr_;
    };
}

#endif // READABLE_MEASURE_HPP