#include <mcmc/measure_processors/readable_measure.hpp>

namespace mcmc::measures {

    ReadableMeasure::ReadableMeasure(const json params) : Parameters(params),
        rel_data_dir_(get_entry<std::string>("rel_data_dir", "./data/"))
    {}

    ReadableMeasure::ReadableMeasure(std::string rel_data_dir) :
        ReadableMeasure(
            json{{"rel_data_dir", rel_data_dir}}
        )
    {}
    
    void ReadableMeasure::write_to_file(const std::string &rel_root_dir) const {
        Parameters::write_to_file(rel_root_dir, "readable_measure_params");
    }

    param_helper::params::Parameters ReadableMeasure::build_expanded_raw_parameters() const {
        param_helper::params::Parameters parameters(params_);
        return parameters;
    }

    std::string ReadableMeasure::name() {
        return "readable_measure";
    }

    std::string ReadableMeasure::get_rel_data_dir() const
    {
        // Create folder if not present
        if(!param_helper::fs::direxists(param_helper::proj::project_root() + rel_data_dir_))
        {
            param_helper::fs::generate_directory_if_not_present(rel_data_dir_);
        }
        return rel_data_dir_;
    }

    std::string ReadableMeasure::get_setting_filename() const
    {
        return gen_filename();
    }

    void ReadableMeasure::initialize_run(std::string_view mode, std::string_view running_parameter, const double rp_value)
    {
        mode_ = mode;
        running_parameter_ = running_parameter;
        rp_value_ = rp_value;

        // Prepare file os
        os_ptr_ = std::make_unique<param_helper::fs::Fileos>(param_helper::proj::project_root() + get_rel_data_dir() + "/" + gen_filename() + ".dat");
    }

    void ReadableMeasure::initialize_measurements(std::string_view starting_mode, const std::vector<std::string> &measure_names, const uint rep) const
    {
        auto &os = os_ptr_->get();
        // Write header of output file
        if (rep == 0) {
            if(measure_names.empty())
                os << measure_names[0];
            for (uint i = 1; i < measure_names.size(); i++)
                os << "\t" << measure_names[i];
            if(measure_names.empty())
                os << std::endl;
        }
    }

    void ReadableMeasure::finalize_measurements() const
    {
        auto &os = os_ptr_->get();
        os.flush();
    }

    std::string ReadableMeasure::gen_filename() const
    {
        if(running_parameter_ == "None")
            return mode_;
        else
            return mode_ + "_" + running_parameter_ + "=" + std::to_string(rp_value_);
    }
}
