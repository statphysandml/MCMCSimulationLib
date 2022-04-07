#ifndef SCALARTHEORYGPU_SCALAR_THEORY_HPP
#define SCALARTHEORYGPU_SCALAR_THEORY_HPP


#include "scalar_theory_gpu_helper.hpp"


class ScalarTheoryGPU : public mcmc::simulation::SystemBase<ScalarTheoryGPU>
{
public:
    explicit ScalarTheoryGPU(const json params):
        SystemBase(params),
        // Configuration parameters
        kappa_(get_entry<double>("kappa", 0.4)),
        lambda_(get_entry<double>("lambda", 1.0)),
        dt_(get_entry<double>("dt", 0.1)),
        n_(get_entry<int>("n", 10)),
        m_(get_entry<double>("m", 1.0)),
        dimensions_(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4})),

        // Further member variables
        rand_(std::uniform_real_distribution<double> (0,1))
    {
        n_sites_ = 1;
        dim_mul_.push_back(n_sites_);
        for(auto dim: dimensions_) {
            n_sites_ *= dim;
            dim_mul_.push_back(n_sites_);
        }

        lattice_ = gen_dev_vec(get_size());
        current_lattice_ = gen_dev_vec(get_size());
        momenta_ = gen_dev_vec(get_size());
        current_momenta_ = gen_dev_vec(get_size());
        drift_ = gen_dev_vec(get_size());
        identity_scalar_product_ = gen_dev_vec(get_size());
        neighbour_indices_ = set_nearest_neighbours(set_nearest_neighbours_helper());
    }

    ScalarTheoryGPU(const double kappa=0.4, const double lambda=1.0, std::vector<int> dimensions={4, 4},
        const double dt=0.1, const int n=10, const double m=1.0) : ScalarTheoryGPU(json{
            {"kappa", kappa},
            {"lambda", lambda},
            {"dimensions", dimensions},
            {"dt", dt},
            {"n",  n},
            {"m", m}
    })
    {}

    void initialize(std::string starting_mode)
    {
        initialize_helper(starting_mode, lattice_, s_, rnd_offset, 1.0);
    }

    void update_step(uint measure_interval=1)
    {
        // Hamiltonian Monte Carlo
        update_step_helper(measure_interval, lattice_, current_lattice_, momenta_, current_momenta_, drift_,
                           identity_scalar_product_, s_, neighbour_indices_, rand_, rnd_offset, dt_,
                           n_, m_, kappa_, lambda_);
    }

    uint get_size() const
    {
        return n_sites_;
    }

    double at(int i) const
    {
        return site_helper(lattice_, i);
    }

    double& at(int i)
    {
        std::cout << "Not implemented on for computations on the GPU" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto get_system_representation() const
    {
        return lattice_;
    }

    auto& get_system_representation()
    {
        return lattice_;
    }

    auto action()
    {
        return action_helper(lattice_, drift_, identity_scalar_product_, neighbour_indices_, kappa_, lambda_);
    }

    void initialize_measurements(std::string starting_mode, uint rep=1)
    {}

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;
        for(const auto measure_name: this->measure_names())
        {
            if(measure_name == "Mean")
                results.push_back(mean());
            else if(measure_name == "AbsMean")
                results.push_back(abs_mean());
            else if(measure_name == "SecondMoment")
                results.push_back(second_moment());
            else if(measure_name == "FourthMoment")
                results.push_back(fourth_moment());
            else if(measure_name == "Action")
                results.push_back(action() / get_size());
            else if(measure_name == "Config")
                results.push_back(configuration());
        }
        return results;
    }

    void finalize_measurements(std::string starting_mode, uint rep=1)
    {}

private:
    double kappa_;
    double lambda_;
    double dt_;
    int n_;
    double m_;

    uint n_sites_; // Total number of sites
    std::vector<int> dimensions_; // Different dimensions
    std::vector<int> dim_mul_; // Accumulated different dimensions (by product)

    dev_vec lattice_;
    dev_vec_vec_int neighbour_indices_; // n_neighbours x n_lattice_sites

    dev_vec current_lattice_;
    dev_vec momenta_;
    dev_vec current_momenta_;
    dev_vec drift_;
    dev_vec identity_scalar_product_;
    thrust::device_vector<curandState> s_;
    std::uniform_real_distribution<double> rand_;
    
    double mean()
    {
        return accumulate(lattice_) / double(get_size());
    }

    double abs_mean()
    {
        return std::fabs(accumulate(lattice_) / double(get_size()));
    }

    double second_moment()
    {
        return second_moment_helper(lattice_, identity_scalar_product_);
    }

    double fourth_moment()
    {
        return fourth_moment_helper(lattice_, identity_scalar_product_);
    }

    std::string configuration() const
    {
        std::string config = std::to_string(at(0));
        for (uint site_idx = 1; site_idx < get_size(); site_idx++)
            config += ", " + std::to_string(at(site_idx));
        return config;
    }

    //site, moving dimension, direction
    int neigh_dir(int i, int d, bool dir) const {
        if (dir)
            return (i - i % (dim_mul_[d] * dimensions_[d]) +
                    (i + dim_mul_[d]) % (dim_mul_[d] * dimensions_[d]));
        else
            return (i - i % (dim_mul_[d] * dimensions_[d]) +
                    (i - dim_mul_[d] + dim_mul_[d] * dimensions_[d]) % (dim_mul_[d] * dimensions_[d]));
    }

    thrust::host_vector< thrust::host_vector<int> > set_nearest_neighbours_helper()
    {
        thrust::host_vector< thrust::host_vector<int> > neighbour_indices;
        for (uint d = 0; d < dimensions_.size(); d++)
        {
            thrust::host_vector<int> nn_of_site_right;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_right.push_back(neigh_dir(i, d, true));
            neighbour_indices.push_back(nn_of_site_right);

            thrust::host_vector<int> nn_of_site_left;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_left.push_back(neigh_dir(i, d, false));
            neighbour_indices.push_back(nn_of_site_left);
        }
        return neighbour_indices;
    }
};

#endif //SCALARTHEORYGPU_SCALAR_THEORY_HPP
