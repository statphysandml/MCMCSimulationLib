//
// Created by lukas on 01.03.20.
//

#ifndef MAIN_PLOT_SITE_DISTRIBUTION_HPP
#define MAIN_PLOT_SITE_DISTRIBUTION_HPP

#include "../simulation_lib/parameters.hpp"
#include "../simulation_lib/json.hpp"
using json = nlohmann::json;

class PlotSiteDistributionParameters : public Parameters {
public:
    explicit PlotSiteDistributionParameters(const json params_) : Parameters(params_)
    {
        xkey = get_value_by_key<std::string>("xkey");
        ykey = get_value_by_key<std::string>("ykey");
        rmin_x = get_value_by_key<double>("rmin_x");
        rmax_x = get_value_by_key<double>("rmax_x");
        rmin_y = get_value_by_key<double>("rmin_y");
        rmax_y = get_value_by_key<double>("rmax_y");
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

    void write_to_file(const std::string& root_dir, const std::string& filename) {
        Parameters::write_to_file(root_dir, "plot_site_distribution_params_" + filename);
        std::cout << "\n#############################################\n./Main plot_site_distribution" <<
        " " << filename << " " << root_dir << "\n#############################################\n" << std::endl;
    }

    static std::string name() {
        return "plot_site_distribution";
    }

private:
    std::string xkey;
    std::string ykey;
    double rmin_x;
    double rmax_x;
    double rmin_y;
    double rmax_y;
};

#endif //MAIN_PLOT_SITE_DISTRIBUTION_HPP
