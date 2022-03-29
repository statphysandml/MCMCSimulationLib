#ifndef SCALARTHEORYGPU_SCALAR_THEORY_HPP
#define SCALARTHEORYGPU_SCALAR_THEORY_HPP


#include "scalar_theory_gpu_helper.hpp"


class ScalarTheoryGPU : public mcmc::simulation::SystemBase<ScalarTheoryGPU>
{
public:
    explicit ScalarTheoryGPU(const json params_):
            SystemBase(params_),
            // Configuration parameters
            kappa(get_entry<double>("kappa", 0.4)),
            lambda(get_entry<double>("lambda", 1.0)),
            dt(get_entry<double>("dt", 0.1)),
            n(get_entry<int>("n", 10)),
            m(get_entry<double>("m", 1.0)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4})),

            // Further member variables
            rand(std::uniform_real_distribution<double> (0,1))
    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }

        lattice = gen_dev_vec(get_size());
        current_lattice = gen_dev_vec(get_size());
        momenta = gen_dev_vec(get_size());
        current_momenta = gen_dev_vec(get_size());
        drift = gen_dev_vec(get_size());
        identity_scalar_product = gen_dev_vec(get_size());
        neighbour_indices = set_nearest_neighbours(set_nearest_neighbours_helper());
    }

    ScalarTheoryGPU(const double kappa_=0.4, const double lambda_=1.0, std::vector<int> dimensions_={4, 4},
        const double dt_=0.1, const int n_=10, const double m_=1.0) : ScalarTheoryGPU(json{
            {"kappa", kappa_},
            {"lambda", lambda_},
            {"dimensions", dimensions_},
            {"dt", dt_},
            {"n",  n_},
            {"m", m_}
    })
    {}

    void initialize(std::string starting_mode)
    {
        initialize_helper(starting_mode, lattice, s, rnd_offset, 1.0);
    }

    void update_step(uint measure_interval=1)
    {
        // Hamiltonian Monte Carlo
        update_step_helper(measure_interval, lattice, current_lattice, momenta, current_momenta, drift,
                           identity_scalar_product, s, neighbour_indices, rand, rnd_offset, dt,
                           n, m, kappa, lambda);
    }

    uint get_size() const
    {
        return n_sites;
    }

    double at(int i) const
    {
        return site_helper(lattice, i);
    }

    double& at(int i)
    {
        std::cout << "Not implemented on for computations on the GPU" << std::endl;
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

    auto action()
    {
        return action_helper(lattice, drift, identity_scalar_product, neighbour_indices, kappa, lambda);
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
    double kappa;
    double lambda;
    double dt;
    int n;
    double m;

    uint n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)

    dev_vec lattice;
    dev_vec_vec_int neighbour_indices; // n_neighbours x n_lattice_sites

    dev_vec current_lattice;
    dev_vec momenta;
    dev_vec current_momenta;
    dev_vec drift;
    dev_vec identity_scalar_product;
    thrust::device_vector<curandState> s;
    std::uniform_real_distribution<double> rand;
    
    double mean()
    {
        return accumulate(lattice) / double(get_size());
    }

    double abs_mean()
    {
        return std::fabs(accumulate(lattice) / double(get_size()));
    }

    double second_moment()
    {
        return second_moment_helper(lattice, identity_scalar_product);
    }

    double fourth_moment()
    {
        return fourth_moment_helper(lattice, identity_scalar_product);
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
            return (i - i % (dim_mul[d] * dimensions[d]) +
                    (i + dim_mul[d]) % (dim_mul[d] * dimensions[d]));
        else
            return (i - i % (dim_mul[d] * dimensions[d]) +
                    (i - dim_mul[d] + dim_mul[d] * dimensions[d]) % (dim_mul[d] * dimensions[d]));
    }

    thrust::host_vector< thrust::host_vector<int> > set_nearest_neighbours_helper()
    {
        thrust::host_vector< thrust::host_vector<int> > neighbour_indices_;
        for (uint d = 0; d < dimensions.size(); d++)
        {
            thrust::host_vector<int> nn_of_site_right;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_right.push_back(neigh_dir(i, d, true));
            neighbour_indices_.push_back(nn_of_site_right);

            thrust::host_vector<int> nn_of_site_left;
            for (uint i = 0; i < get_size(); i++)
                nn_of_site_left.push_back(neigh_dir(i, d, false));
            neighbour_indices_.push_back(nn_of_site_left);
        }
        return neighbour_indices_;
    }
};

#endif //SCALARTHEORYGPU_SCALAR_THEORY_HPP
