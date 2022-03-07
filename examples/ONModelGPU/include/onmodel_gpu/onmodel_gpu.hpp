//
// Created by lukas on 02.03.22.
//

#ifndef ONMODELGPU_SCALAR_HPP
#define ONMODELGPU_SCALAR_HPP


#include "onmodel_gpu_helper.hpp"


template<uint N>
class ONModelGPU;


template<uint N>
struct ONModelGPUParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit ONModelGPUParameters(const json params_):
            SystemBaseParameters(params_),
            kappa(get_entry<double>("kappa", 0.4)),
            lambda(get_entry<double>("lambda", 1.0)),
            epsilon(get_entry<double>("epsilon", 0.01)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))
    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }
    }

    ONModelGPUParameters(const double kappa_, const double lambda_, std::vector<int> dimensions_, const double epsilon_) : ONModelGPUParameters(json{
            {"kappa", kappa_},
            {"lambda", lambda_},
            {"dimensions", dimensions_},
            {"epsilon", epsilon_}
    })
    {}

    std::unique_ptr<ONModelGPU<N>> generate() { return std::make_unique<ONModelGPU<N>>(*this); }

    typedef ONModelGPU<N> Model;

    friend class ONModelGPU<N>;

    const double kappa;
    const double lambda;
    double epsilon;

    uint n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};


template<uint N>
class ONModelGPU : public mcmc::simulation::SystemBase<ONModelGPU<N>>
{
public:
    explicit ONModelGPU(const ONModelGPUParameters<N> &mp_) :
            mp(mp_)
    {
        lattice = gen_lattice(N, get_size());
        neighbour_indices = set_nearest_neighbours(set_nearest_neighbours_helper());
    }

    void initialize(std::string starting_mode)
    {
        initialize_helper(starting_mode, N, lattice, s, get_size(), rnd_offset, mp.epsilon);
    }

    void update_step(uint measure_interval=1)
    {
        // Langevin update
        update_step_helper(measure_interval, N, lattice, s, neighbour_indices,
                           get_size(), rnd_offset, mp.epsilon, mp.kappa, mp.lambda);
    }

    uint get_size() const
    {
        return mp.n_sites;
    }

    lm_impl::link::ON<double, N> at(int i) const
    {
        std::vector<double> site = site_helper(lattice, i);
        return lm_impl::link::ON<double, N>(site);
    }

    lm_impl::link::ON<double, N>& at(int i)
    {
        std::cout << "Not implemented for computations on the GPU" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto get_system_representation() const
    {
        return lattice;
    }

    auto& get_system_representation()
    {
        return lattice;
    }

    auto action() const
    {
        return action_helper(N, lattice, neighbour_indices, get_size(), mp.kappa, mp.lambda);
    }

    void initialize_measurements(std::string starting_mode, uint rep=1)
    {}

    auto perform_measurements()
    {
        std::vector<std::variant<int, double, std::string>> results;

        for(const auto measure_name: get_measure_names())
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

    std::vector<std::string> get_measure_names()
    {
        return mp.get_measures();
    }

private:
    DevDatC lattice; // N * n_lattice_sites, stored as flattened N x n_lattice_sites matrix
    dev_vec_vec_int neighbour_indices; // n_neighbours x (N * n_lattice_sites)
    thrust::device_vector<curandState> s;

    const ONModelGPUParameters<N> &mp;

    std::string mean()
    {
        return std::to_string(lm_impl::link::ON<double, N>(accumulate(N, lattice, get_size())) / double(get_size()));
    }

    double abs_mean()
    {
        return std::fabs(lm_impl::link::ON<double, N>(accumulate(N, lattice, get_size())) / double(get_size()));
    }

    double second_moment()
    {
        return second_moment_helper(N, lattice, get_size());
    }

    double fourth_moment()
    {
        return fourth_moment_helper(N, lattice, get_size());
    }

    std::string configuration() const
    {
        std::string config = std::to_string(at(0));
        for (uint site_idx = 1; site_idx < get_size(); site_idx++)
            config += ", " + std::to_string(at(site_idx));
        return config;
    }

    //site, moving dimension, direction
    int neigh_dir(int n, int d, bool dir) const {
        if (dir)
            return (n - n % (mp.dim_mul[d] * mp.dimensions[d]) +
                    (n + mp.dim_mul[d]) % (mp.dim_mul[d] * mp.dimensions[d]));
        else
            return (n - n % (mp.dim_mul[d] * mp.dimensions[d]) +
                    (n - mp.dim_mul[d] + mp.dim_mul[d] * mp.dimensions[d]) % (mp.dim_mul[d] * mp.dimensions[d]));
    }

    thrust::host_vector< thrust::host_vector<int> > set_nearest_neighbours_helper()
    {
        thrust::host_vector< thrust::host_vector<int> > neighbour_indices_;
        for (uint d = 0; d < mp.dimensions.size(); d++)
        {
            thrust::host_vector<int> nn_of_site_right;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_right.push_back(neigh_dir(i, d, true));
            for (uint elem = 1; elem < N; elem++) {
                for(uint i = 0; i < get_size(); i++)
                    nn_of_site_right.push_back(nn_of_site_right[i] + elem * get_size());
            }
            neighbour_indices_.push_back(nn_of_site_right);

            thrust::host_vector<int> nn_of_site_left;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_left.push_back(neigh_dir(i, d, false));
            for (uint elem = 1; elem < N; elem++) {
                for(uint i = 0; i < get_size(); i++)
                    nn_of_site_left.push_back(nn_of_site_left[i] + elem * get_size());
            }
            neighbour_indices_.push_back(nn_of_site_left);
        }
        return neighbour_indices_;
    }
};

#endif //ONMODELGPU_SCALAR_HPP
