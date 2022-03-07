#ifndef SCALARTHEORYGPU_SCALAR_THEORY_HPP
#define SCALARTHEORYGPU_SCALAR_THEORY_HPP


#include "scalar_theory_gpu_helper.hpp"


class ScalarTheoryGPU;


struct ScalarTheoryGPUParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit ScalarTheoryGPUParameters(const json params_):
            SystemBaseParameters(params_),
            kappa(get_entry<double>("kappa", 0.4)),
            lambda(get_entry<double>("lambda", 1.0)),
            dt(get_entry<double>("dt", 0.01)),
            n(get_entry<int>("n", 20)),
            m(get_entry<double>("m", 1.0)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))
    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }
    }

    ScalarTheoryGPUParameters(const double kappa_, const double lambda_, std::vector<int> dimensions_,
        const double dt_, const int n_, const double m_=1.0) : ScalarTheoryGPUParameters(json{
            {"kappa", kappa_},
            {"lambda", lambda_},
            {"dimensions", dimensions_},
            {"dt", dt_},
            {"n",  n_},
            {"m", m_}
    })
    {}

    std::unique_ptr<ScalarTheoryGPU> generate() { return std::make_unique<ScalarTheoryGPU>(*this); }

    typedef ScalarTheoryGPU Model;

    const double kappa;
    const double lambda;
    const double dt;
    const int n;
    const double m;

    uint n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};


class ScalarTheoryGPU : public mcmc::simulation::SystemBase<ScalarTheoryGPU>
{
public:
    explicit ScalarTheoryGPU(const ScalarTheoryGPUParameters &mp_) :
            mp(mp_),
            rand(std::uniform_real_distribution<double> (0,1))
    {
        lattice = gen_dev_vec(get_size());
        current_lattice = gen_dev_vec(get_size());
        momenta = gen_dev_vec(get_size());
        current_momenta = gen_dev_vec(get_size());
        drift = gen_dev_vec(get_size());
        identity_scalar_product = gen_dev_vec(get_size());
        neighbour_indices = set_nearest_neighbours(set_nearest_neighbours_helper());
    }

    void initialize(std::string starting_mode)
    {
        initialize_helper(starting_mode, lattice, s, rnd_offset, 1.0);
    }

    void update_step(uint measure_interval=1)
    {
        // Hamiltonian Monte Carlo
        update_step_helper(measure_interval, lattice, current_lattice, momenta, current_momenta, drift,
                           identity_scalar_product, s, neighbour_indices, rand, rnd_offset, mp.dt,
                           mp.n, mp.m, mp.kappa, mp.lambda);
    }

    uint get_size() const
    {
        return mp.n_sites;
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
        return action_helper(lattice, drift, identity_scalar_product, neighbour_indices, mp.kappa, mp.lambda);
    }

    /* void initialize_measurements(std::string starting_mode, uint rep=1)
    {} */

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

    /* void finalize_measurements(std::string starting_mode, uint rep=1)
    {} */

    std::vector<std::string> get_measure_names()
    {
        return mp.get_measures();
    }

private:
    dev_vec lattice;
    dev_vec_vec_int neighbour_indices; // n_neighbours x n_lattice_sites

    const ScalarTheoryGPUParameters &mp;

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
