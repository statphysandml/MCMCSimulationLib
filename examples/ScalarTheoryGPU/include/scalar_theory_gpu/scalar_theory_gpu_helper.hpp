#ifndef SCALARTHEORYGPU_SCALAR_THEORY_GPU_HELPER_HPP
#define SCALARTHEORYGPU_SCALAR_THEORY_GPU_HELPER_HPP

#include <mcmc/mcmc_simulation/util/random.hpp>

#include <mcmc/mcmc_simulation/systembase_parameters.hpp>
#include <mcmc/mcmc_simulation/systembase.hpp>

#include "../thrust/thrust_header.hpp"
#include "../thrust/thrust_functors.hpp"

#include <curand_kernel.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/inner_product.h>

static unsigned long long rnd_offset = 0;

dev_vec gen_dev_vec(const uint n_sites);

dev_vec_vec_int set_nearest_neighbours(thrust::host_vector< thrust::host_vector<int> > neighbour_indices);

double site_helper(const dev_vec &lattice, int i);

void initialize_helper(std::string starting_mode, dev_vec &lattice, thrust::device_vector<curandState> &s,
                       unsigned long long &rnd_offset, const double epsilon);

void update_step_helper(uint measure_interval, dev_vec &lattice, dev_vec &current_lattice, dev_vec &momenta,
                        dev_vec &current_momenta, dev_vec &drift, dev_vec &identity_scalar_product,
                        thrust::device_vector<curandState> &s, dev_vec_vec_int &neighbour_indices,
                        std::uniform_real_distribution<double> &rand, unsigned long long &rnd_offset, const double dt,
                        const int n, const double m, const double kappa, const double lambda);

void compute_square(const dev_vec &lattice, dev_vec &identity_scalar_product);

void drift_computation(const dev_vec &lattice, dev_vec &drift, dev_vec &identity_scalar_product,
                       const dev_vec_vec_int& neighbour_indices, unsigned long long &rnd_offset,
                       const double kappa, const double lambda);

double action_helper(const dev_vec &lattice, dev_vec& potential, dev_vec &identity_scalar_product,
                     const dev_vec_vec_int& neighbour_indices, const double kappa, const double lambda);


// Helper functions for computing observables

// Sum
double accumulate(const dev_vec &lattice);

// Second moment
double second_moment_helper(const dev_vec &lattice, dev_vec &identity_scalar_product);

// Fourth moment
double fourth_moment_helper(const dev_vec &lattice, dev_vec &identity_scalar_product);


// Structs used by the helper functions implementing parallel computations on the GPU

/* https://stackoverflow.com/questions/40142493/using-curand-inside-a-thrust-functor */

// Functor for initializing the random generators on the GPU
struct curand_setup
{
    const unsigned long long offset;

    curand_setup(const unsigned long long offset) :
            offset(offset)
    {}

    using init_tuple = thrust::tuple<int, curandState&>;
    __device__
    void operator()(init_tuple t){
        curandState s;
        unsigned int seed = thrust::get<0>(t);
        // seed a random number generator
        curand_init(seed, 0, offset, &s);
        thrust::get<1>(t) = s;
    }
};


// Functor for initializing the lattice sites with random Gaussian noise
struct initialization
{
    const double sqrt2epsilon;

    initialization(const double epsilon) :
            sqrt2epsilon(std::sqrt(2.0 * epsilon))
    {}

    using Tuple = thrust::tuple<curandState&, cudaT&>;
    __device__
    void operator()(Tuple t) {
        thrust::get<1>(t) = sqrt2epsilon * curand_normal(&thrust::get<0>(t));
    }
};


// Functor for initializing the lattice sites with random Gaussian noise
struct momentum_initialization
{
    using Tuple = thrust::tuple<curandState&, cudaT&>;
    __device__
    void operator()(Tuple t) {
        thrust::get<1>(t) = curand_normal(&thrust::get<0>(t));
    }
};


// Functor used to sum up neighbour contributions
struct drift_interaction
{
    template <typename Tuple>
    __host__ __device__
    void operator()(Tuple t) {
        thrust::get<0>(t) += thrust::get<1>(t);
    }
};

// Used to evaluate the scalar product of elements of the O(N) group
template <typename T>
struct transpose : public thrust::unary_function<T,T>
{
    T R; // number of rows
    T C; // number of columns

    __host__ __device__
    transpose(T R, T C) : R(R), C(C) {}

    __host__ __device__
    T operator()(T i)
    {
        return (i % R) * C + i / R;
    }
};


// Used to evaluate the scalar product of elements of the O(N) group
struct square : public thrust::unary_function<float,float>
{
    __host__ __device__
    float operator()(float x) const
    {
        return x * x;
    }
};


// Compute the interacton term in the action of the scalar theory
struct action_interaction
{
    template <typename Tuple>
    __host__ __device__
    void operator()(Tuple t) {
        thrust::get<0>(t) += thrust::get<1>(t) * thrust::get<2>(t);
    }
};


// Compute the drift of the scalar theory
struct compute_drift
{
    const double kappa;
    const double lambda;

    compute_drift(const double kappa, const double lambda) :
            kappa(kappa), lambda(lambda)
    {}

    using tuple = thrust::tuple<cudaT, cudaT&, cudaT>;
    __device__
    void operator()(tuple t) {
        // drift = -2.0 * mp.kappa * potential + 2.0 * site + 4.0 * mp.lambda * (site * site - 1.0) * site;
        thrust::get<1>(t) = -2.0 * kappa * thrust::get<1>(t) + 2.0 * thrust::get<0>(t) + 4.0 * lambda * (thrust::get<2>(t) - 1.0) * thrust::get<0>(t);
    }
};


// Compute the action of the scalar theory based on nearest neighbour potential and the scalar product for each site
struct compute_action
{
    const double kappa;
    const double lambda;

    compute_action(const double kappa, const double lambda) :
            kappa(kappa), lambda(lambda)
    {}

    __host__ __device__
    cudaT operator()(const cudaT& potential, const cudaT& site_sq) const {
        return -2.0 * kappa * potential + site_sq + lambda * (site_sq - 1.0) * (site_sq - 1.0);
    }
};


// Momentum leapfrog update
struct leapfrog_momenta_update : public thrust::binary_function<float,float,float>
{
    const double dt;

    leapfrog_momenta_update(const double dt_) : dt(dt_)
    {}

    __host__ __device__
    float operator()(float momentum, float drift) const
    {
        return momentum - dt / 2.0 * drift;
    }
};


// Lattice leapfrog update
struct leapfrog_lattice_update : public thrust::binary_function<float,float,float>
{
    const double dt;
    const double m;

    leapfrog_lattice_update(const double dt_, const double m_) : dt(dt_), m(m_)
    {}

    __host__ __device__
    float operator()(float site, float momentum) const
    {
        return site + dt * momentum / m;
    }
};

//* Implementation of a langevin update *//

/* // Performs the Langevin update based on the interaction contribution of neighbours and the computed scalar product for
// each lattice site
struct langevin_update
{
    const double epsilon;
    const double sqrt2epsilon;
    const double kappa;
    const double lambda;

    langevin_update(const double epsilon, const double kappa, const double lambda) :
            epsilon(epsilon), sqrt2epsilon(std::sqrt(2.0 * epsilon)), kappa(kappa), lambda(lambda)
    {}

    using tuple = thrust::tuple<curandState&, cudaT&, cudaT, cudaT>;
    __device__
    void operator()(tuple t) {
        // drift = -2.0 * mp.kappa * potential + 2.0 * site + 4.0 * mp.lambda * (site * site - 1.0) * site;
        auto drift = -2.0 * kappa * thrust::get<2>(t) + 2.0 * thrust::get<1>(t) + 4.0 * lambda * (thrust::get<3>(t) - 1.0) * thrust::get<1>(t);
        thrust::get<1>(t) = thrust::get<1>(t) - epsilon * drift + sqrt2epsilon * curand_normal(&thrust::get<0>(t));
    }
}; */


#endif //SCALARTHEORYGPU_SCALAR_THEORY_GPU_HELPER_HPP
