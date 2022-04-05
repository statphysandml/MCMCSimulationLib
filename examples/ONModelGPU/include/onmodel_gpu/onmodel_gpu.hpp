#ifndef ONMODELGPU_SCALAR_HPP
#define ONMODELGPU_SCALAR_HPP


#include <mcmc_simulation/header.hpp>

#include "onmodel_gpu_helper.hpp"


template<uint N>
class ONModelGPU : public mcmc::simulation::SystemBase<ONModelGPU<N>>
{
public:
    explicit ONModelGPU(const json params):
            mcmc::simulation::SystemBase<ONModelGPU<N>>(params),
            // Configuration parameters
            kappa_(this-> template get_entry<double>("kappa", 0.4)),
            lambda_(this-> template get_entry<double>("lambda", 1.0)),
            epsilon_(this-> template get_entry<double>("epsilon", 0.01)),
            dimensions_(this-> template get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))
    {
        n_sites_ = 1;
        dim_mul_.push_back(n_sites_);
        for(auto dim: dimensions_) {
            n_sites_ *= dim;
            dim_mul_.push_back(n_sites_);
        }

        lattice_ = gen_lattice(N, get_size());
        neighbour_indices_ = set_nearest_neighbours(set_nearest_neighbours_helper());
    }

    ONModelGPU(const double kappa=0.4, const double lambda=1.0, std::vector<int> dimensions={4, 4}, const double epsilon=0.01) : ONModelGPU(json{
            {"kappa", kappa},
            {"lambda", lambda},
            {"dimensions", dimensions},
            {"epsilon", epsilon}
    })
    {}

    void initialize(std::string starting_mode)
    {
        initialize_helper(starting_mode, N, lattice_, s_, get_size(), rnd_offset, epsilon_);
    }

    void update_step(uint measure_interval=1)
    {
        // Langevin update
        update_step_helper(measure_interval, N, lattice_, s_, neighbour_indices_,
                           get_size(), rnd_offset, epsilon_, kappa_, lambda_);
    }

    uint get_size() const
    {
        return n_sites_;
    }

    lm_impl::link::ON<double, N> at(int i) const
    {
        std::vector<double> site = site_helper(lattice_, i);
        return lm_impl::link::ON<double, N>(site);
    }

    lm_impl::link::ON<double, N>& at(int i)
    {
        std::cout << "Not implemented for computations on the GPU" << std::endl;
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

    auto action() const
    {
        return action_helper(N, lattice_, neighbour_indices_, get_size(), kappa_, lambda_);
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
    double epsilon_;

    uint n_sites_; // Total number of sites
    std::vector<int> dimensions_; // Different dimensions
    std::vector<int> dim_mul_; // Accumulated different dimensions (by product)

    DevDatC lattice_; // N * n_lattice_sites, stored as flattened N x n_lattice_sites matrix
    dev_vec_vec_int neighbour_indices_; // n_neighbours x (N * n_lattice_sites)
    thrust::device_vector<curandState> s_;

    std::string mean()
    {
        return std::to_string(lm_impl::link::ON<double, N>(accumulate(N, lattice_, get_size())) / double(get_size()));
    }

    double abs_mean()
    {
        return std::fabs(lm_impl::link::ON<double, N>(accumulate(N, lattice_, get_size())) / double(get_size()));
    }

    double second_moment()
    {
        return second_moment_helper(N, lattice_, get_size());
    }

    double fourth_moment()
    {
        return fourth_moment_helper(N, lattice_, get_size());
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
            for (uint elem = 1; elem < N; elem++) {
                for(uint i = 0; i < get_size(); i++)
                    nn_of_site_right.push_back(nn_of_site_right[i] + elem * get_size());
            }
            neighbour_indices.push_back(nn_of_site_right);

            thrust::host_vector<int> nn_of_site_left;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_left.push_back(neigh_dir(i, d, false));
            for (uint elem = 1; elem < N; elem++) {
                for(uint i = 0; i < get_size(); i++)
                    nn_of_site_left.push_back(nn_of_site_left[i] + elem * get_size());
            }
            neighbour_indices.push_back(nn_of_site_left);
        }
        return neighbour_indices;
    }
};

#endif //ONMODELGPU_SCALAR_HPP
