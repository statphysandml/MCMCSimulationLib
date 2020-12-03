//
// Created by lukas on 01.03.20.
//

#ifndef MAIN_PLOT_SITE_DISTRIBUTION_HPP
#define MAIN_PLOT_SITE_DISTRIBUTION_HPP

#include "param_helper/params.hpp"
#include "param_helper/json.hpp"

using json = nlohmann::json;

class PlotSiteDistributionParameters : public impl_helper::params::Parameters {
public:
    explicit PlotSiteDistributionParameters(const json params_) : Parameters(params_)
    {
        xkey = get_entry<std::string>("xkey", "x");
        ykey = get_entry<std::string>("ykey", "y");
        rmin_x = get_entry<double>("rmin_x", -1.0);
        rmax_x = get_entry<double>("rmax_x", 1.0);
        rmin_y = get_entry<double>("rmin_y", -1.0);
        rmax_y = get_entry<double>("rmax_y", 1.0);
    }

    PlotSiteDistributionParameters(
            std::string xkey_,
            std::string ykey_,
            double rmin_x_,
            double rmax_x_,
            double rmin_y_,
            double rmax_y_
    ) : PlotSiteDistributionParameters(
            json {{"xkey", xkey_},
                  {"ykey", ykey_},
                  {"rmin_x", rmin_x_},
                  {"rmax_x", rmax_x_},
                  {"rmin_y", rmin_y_},
                  {"rmax_y", rmax_y_}})
    {}

    void write_to_file(const std::string& root_dir) {
        Parameters::write_to_file(root_dir, "plot_site_distribution_params");
    }

    Parameters build_expanded_raw_parameters() const
    {
        Parameters parameters(params);
        return parameters;
    }

    static std::string name() {
        return "plot_site_distribution";
    }

    std::unique_ptr<MarkovChainParameters> generate_markovchain_params(std::string running_parameter="None", double rp=0)
    {
        return std::make_unique<MarkovChainParameters>(1, 0, 0, 0, "hot");
    }

    json& get_measures()
    {
        return dummy;
    }

private:
    std::string xkey;
    std::string ykey;
    double rmin_x;
    double rmax_x;
    double rmin_y;
    double rmax_y;

    json dummy;
};

#endif //MAIN_PLOT_SITE_DISTRIBUTION_HPP
