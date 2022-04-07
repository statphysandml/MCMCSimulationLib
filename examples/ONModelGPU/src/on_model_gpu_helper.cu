#include "../include/onmodel_gpu/onmodel_gpu_helper.hpp"


DevDatC gen_lattice(const uint N, const uint dim)
{
    return DevDatC(N, dim);
}


dev_vec_vec_int set_nearest_neighbours(thrust::host_vector< thrust::host_vector<int> > neighbour_indices)
{
    dev_vec_vec_int neighbour_indices_dev;
    for(auto& nn_of_site : neighbour_indices) {
        dev_vec_int nn_of_site_dev = nn_of_site;
        neighbour_indices_dev.push_back(nn_of_site_dev);
    }
    return std::move(neighbour_indices_dev);
}


std::vector<double> site_helper(const DevDatC &lattice, int i)
{
    return lattice.get_ith_element(i);
}


void initialize_helper(std::string starting_mode, uint N, DevDatC &lattice, thrust::device_vector<curandState> &s,
                       uint n_sites, unsigned long long &rnd_offset, const double epsilon)
{
    if(starting_mode == "hot") {
        s = thrust::device_vector<curandState>(lattice.size());

        rnd_offset = 0;

        // Initialize the random generators
        thrust::for_each_n(thrust::make_zip_iterator(thrust::make_tuple(thrust::counting_iterator<int>(0), s.begin())),
                           lattice.size(), curand_setup(rnd_offset));

        // Initialize each lattice site with a random Gaussian number, where 2 * epsilon refers to the standard deviation
        thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(s.begin(), lattice.begin())),
                         thrust::make_zip_iterator(thrust::make_tuple(s.end(), lattice.end())),
                         initialization(epsilon));
        rnd_offset += 1;
    }
    else
        thrust::fill(lattice.begin(), lattice.end(), 0.0);
    // print_range("Initialized Lattice", lattice.begin(), lattice.end());
    // std::cout << " Expected lattice size " << n_sites * N << " = Actual lattice " << lattice.size() << std::endl;
}


void update_step_helper(uint measure_interval, uint N, DevDatC &lattice, thrust::device_vector<curandState> &s, dev_vec_vec_int &neighbour_indices,
                        uint n_sites, unsigned long long &rnd_offset, const double epsilon, const double kappa, const double lambda)
{
    for(uint i = 0; i < measure_interval; i++)
    {
        // Computes for each phi_i the drift term:
        //     -2.0 * kappa \sum_neighbours \phi_j + 2.0 * (1 - 2.0 * lambda) phi_i + 4.0 * lambda * phi_i^3,
        // where the sum runs over all neighbours of phi_i and performs a Langevin update.

        //[ Compute interaction term \sum_neighbours \phi_neighbour

        dev_vec interaction_term(n_sites * N, 0.0);

        // Sum over neighbours for each site AND each component by looping over all neighbours
        for(size_t n = 0; n < neighbour_indices.size(); n++) {
            thrust::for_each(
                    thrust::make_zip_iterator(thrust::make_tuple(
                            interaction_term.begin(), thrust::make_permutation_iterator(lattice.begin(), neighbour_indices[n].begin()))),
                    thrust::make_zip_iterator(thrust::make_tuple(
                            interaction_term.end(), thrust::make_permutation_iterator(lattice.end(), neighbour_indices[n].end()))),
                    drift_interaction()
            );
        }
        //]

        //[ Compute the scalar product \phi^2

        // Compute the inner product
        dev_vec identity_scalar_product = compute_inner_product(N, lattice, n_sites);

        // Tile to to have the scalar for each component
        tiled_range<dev_iterator> identity_scalar_product_tiled(
                identity_scalar_product.begin(), identity_scalar_product.end(), N);
        //]

        //[ Add remaining terms and update sites and add noise
        thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(s.begin(), lattice.begin(), interaction_term.begin(), identity_scalar_product_tiled.begin())),
                         thrust::make_zip_iterator(thrust::make_tuple(s.end(), lattice.end(), interaction_term.end(), identity_scalar_product_tiled.end())),
                         langevin_update(epsilon, kappa, lambda));
        // Update offset for random number generation
        rnd_offset += 1;
        //]
    }
}


// Computes the scalar product separately for each lattice site
dev_vec compute_inner_product(const uint N, const DevDatC &lattice, const uint n_sites)
{
    dev_vec identity_scalar_product(n_sites, 0.0);
    dev_vec identity_scalar_product_indices(n_sites);

    // Compute the square of each field an perform for each single field a sum over the components
    // keys = 0 0 0 0 1 1 1 1 2 2 2 2 ...
    // mask = 0 16 32 48 1 17 33 49 2 18 34 50 ... (as an example for a 4x4 lattice)
    thrust::reduce_by_key
            (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(N)),
             thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(N)) + (N * n_sites),
             thrust::make_permutation_iterator(
                     thrust::make_transform_iterator(lattice.begin(), square()),
                     thrust::make_transform_iterator(thrust::counting_iterator<int>(0), transpose<int>(N, n_sites))),
             identity_scalar_product_indices.begin(),
             identity_scalar_product.begin(),
             thrust::equal_to<int>(),
             thrust::plus<cudaT>());
    return std::move(identity_scalar_product);
}


double action_helper(const uint N, const DevDatC &lattice, const dev_vec_vec_int& neighbour_indices,
                     const uint n_sites, const double kappa, const double lambda)
{
    // Computes the action of the scalar theory according to
    //     S = -2.0 * kappa \sum_<i, j> \phi_i \phi_j + \sum_i (1 - 2.0 * lambda) phi_i^2 + lambda * phi_i^4
    // where the sum runs over neighbour pairs phi_i (note that in the computation only the right hand side neighbours
    // need to be taken into account.

    dev_vec potential(n_sites * N, 0.0);

    //[ Compute interaction term \sum_neighbours \phi * \phi_neighbour

    // Sum over neighbours for each site AND each component
    for(size_t n = 0; n < neighbour_indices.size(); n +=2) // only positive neighbours!!
    {
        thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(
                potential.begin(), lattice.begin(), thrust::make_permutation_iterator(lattice.begin(), neighbour_indices[n].begin()))),
                         thrust::make_zip_iterator(thrust::make_tuple(
                                 potential.end(), lattice.end(), thrust::make_permutation_iterator(lattice.end(), neighbour_indices[n].end()))),
                         action_interaction());
    }
    //]

    //[ Perform sum over components of interaction (Reduce from N * n_sites to n_sites by executing the some of the inner product \phi_i \phi_j)

    dev_vec potential_scalar_product_indices(n_sites);

    thrust::reduce_by_key
            // The first line does the same as https://github.com/NVIDIA/thrust/blob/main/examples/repeated_range.cu
            (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(N)),
             thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(N)) + (N * n_sites),
             thrust::make_permutation_iterator(
                     potential.begin(),
                     thrust::make_transform_iterator(thrust::counting_iterator<int>(0), transpose<int>(N, n_sites))),
             potential_scalar_product_indices.begin(),
             potential.begin(),
             thrust::equal_to<int>(),
             thrust::plus<cudaT>());

    potential.resize(n_sites);
    //]

    //[ Compute the scalar product \phi^2
    dev_vec identity_scalar_product = compute_inner_product(N, lattice, n_sites);
    //]

    //[ Add remaining terms and compute sum

    // <-> potential = -2.0 * mp.kappa * potential + site * site + mp.lambda * pow(site * site - 1.0, 2.0);
    thrust::transform(potential.begin(), potential.end(), identity_scalar_product.begin(), potential.begin(), compute_action(kappa, lambda));
    return thrust::reduce(potential.begin(), potential.end());
    //]
}


// Accumulates each component over the lattice
std::vector<double> accumulate(const uint N, const DevDatC &lattice, const uint n_sites)
{
    dev_vec sum(N, 0.0);
    dev_vec sum_indices(N);

    thrust::reduce_by_key
            // The first line does the same as https://github.com/NVIDIA/thrust/blob/main/examples/repeated_range.cu
            (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(n_sites)),
             thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(n_sites)) + (N * n_sites),
             lattice.begin(),
             sum_indices.begin(),
             sum.begin(),
             thrust::equal_to<int>(),
             thrust::plus<cudaT>());

    std::vector<double> sum_vec(N, 0);
    thrust::copy(sum.begin(), sum.end(), sum_vec.begin());
    return sum_vec;
}


// Second moment
double second_moment_helper(const uint N, const DevDatC &lattice, const uint n_sites)
{
    dev_vec identity_scalar_product = compute_inner_product(N, lattice, n_sites);
    return thrust::reduce(identity_scalar_product.begin(), identity_scalar_product.end()) / n_sites;
}


// Fourth moment
double fourth_moment_helper(const uint N, const DevDatC &lattice, const uint n_sites)
{
    dev_vec identity_scalar_product = compute_inner_product(N, lattice, n_sites);
    return thrust::inner_product(identity_scalar_product.begin(), identity_scalar_product.end(), identity_scalar_product.begin(), 0.0f) / n_sites;
}