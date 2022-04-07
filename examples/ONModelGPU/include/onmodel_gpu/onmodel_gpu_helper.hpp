#ifndef ONMODELGPU_SCALAR_THEORY_GPU_HPP
#define ONMODELGPU_SCALAR_THEORY_GPU_HPP


#include "on.hpp"

#include "../thrust/thrust_header.hpp"
#include "../thrust/thrust_functors.hpp"
#include "../thrust/dev_dat.hpp"

#include <curand_kernel.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/inner_product.h>

static unsigned long long rnd_offset = 0;

DevDatC gen_lattice(const uint N, const uint dim);

dev_vec_vec_int set_nearest_neighbours(thrust::host_vector< thrust::host_vector<int> > neighbour_indices);

std::vector<double> site_helper(const DevDatC &lattice, int i);

void initialize_helper(std::string starting_mode, uint N, DevDatC &lattice, thrust::device_vector<curandState> &s,
                       uint n_sites, unsigned long long &rnd_offset, const double epsilon);

void update_step_helper(uint measure_interval, uint N, DevDatC &lattice, thrust::device_vector<curandState> &s, dev_vec_vec_int& neighbour_indices,
                        uint n_sites, unsigned long long &rnd_offset, const double epsilon, const double kappa, const double lambda);

dev_vec compute_inner_product(const uint N, const DevDatC &lattice, const uint n_sites);

double action_helper(const uint N, const DevDatC &lattice, const dev_vec_vec_int& neighbour_indices,
                     const uint n_sites, const double kappa, const double lambda);


// Helper functions for computing observables

// Sum
std::vector<double> accumulate(const uint N, const DevDatC &lattice, const uint n_sites);

// Second moment
double second_moment_helper(const uint N, const DevDatC &lattice, const uint n_sites);

// Fourth moment
double fourth_moment_helper(const uint N, const DevDatC &lattice, const uint n_sites);


// Structs used by the helper functions implementing parallel computations on the GPU


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


// Performs the Langevin update based on the interaction contribution of neighbours and the computed scalar product for
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


#endif //ONMODELGPU_SCALAR_THEORY_GPU_HPP
