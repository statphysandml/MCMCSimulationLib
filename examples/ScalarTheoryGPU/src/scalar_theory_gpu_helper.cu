#include "../include/scalar_theory_gpu/scalar_theory_gpu_helper.hpp"


dev_vec gen_dev_vec(const uint n_sites)
{
    return dev_vec(n_sites, 0.0);
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


double site_helper(const dev_vec &lattice, int i)
{
    return lattice[i];
}


void initialize_helper(std::string starting_mode, dev_vec &lattice, thrust::device_vector<curandState> &s,
                       unsigned long long &rnd_offset, const double epsilon)
{
    if(starting_mode == "hot") {
        s = thrust::device_vector<curandState>(lattice.size());

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
}


void update_step_helper(uint measure_interval, dev_vec &lattice, dev_vec &current_lattice, dev_vec &momenta,
                        dev_vec &current_momenta, dev_vec &drift, dev_vec &identity_scalar_product,
                        thrust::device_vector<curandState> &s, dev_vec_vec_int &neighbour_indices,
                        std::uniform_real_distribution<double> &rand, unsigned long long &rnd_offset, const double dt,
                        const int n, const double m, const double kappa, const double lambda)
{
    for(uint i = 0; i < measure_interval; i++)
    {
        auto current_action = action_helper(lattice, drift, identity_scalar_product, neighbour_indices, kappa, lambda);
        thrust::copy(lattice.begin(), lattice.end(), current_lattice.begin());

        // Sample momenta
        thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(s.begin(), momenta.begin())),
                         thrust::make_zip_iterator(thrust::make_tuple(s.end(), momenta.end())),
                         momentum_initialization());
        // Update offset for random number generation
        rnd_offset += 1;

        thrust::copy(momenta.begin(), momenta.end(), current_momenta.begin());

        // Hamiltonians' equation - Leapfrog
        for(auto i = 0; i < n; i++)
        {
            drift_computation(lattice, drift, identity_scalar_product, neighbour_indices, rnd_offset, kappa, lambda);
            thrust::transform(momenta.begin(), momenta.end(), drift.begin(), momenta.begin(), leapfrog_momenta_update(dt));

            thrust::transform(lattice.begin(), lattice.end(), momenta.begin(), lattice.begin(), leapfrog_lattice_update(dt, m));

            drift_computation(lattice, drift, identity_scalar_product, neighbour_indices, rnd_offset, kappa, lambda);
            thrust::transform(momenta.begin(), momenta.end(), drift.begin(), momenta.begin(), leapfrog_momenta_update(dt));
        }

        auto proposal_action = action_helper(lattice, drift, identity_scalar_product, neighbour_indices, kappa, lambda);

        auto current_kinetic_term = thrust::inner_product(current_momenta.begin(), current_momenta.end(), current_momenta.begin(), 0.0);
        auto proposal_kinetic_term = thrust::inner_product(momenta.begin(), momenta.end(), momenta.begin(), 0.0);

        // std::cout << proposal_action + 0.5 / m * proposal_kinetic_term << " == " << current_action + 0.5 / m * current_kinetic_term << std::endl;

        // Accept/Reject step
        if (rand(mcmc::util::g_gen) >= std::min(1.0, std::exp(
                -1.0 * (proposal_action - current_action) - 0.5 * (proposal_kinetic_term - current_kinetic_term) / m))) {
            thrust::copy(current_lattice.begin(), current_lattice.end(), lattice.begin()); // Reject
        }
        // else{} // Accept
    }
}


// Computes the scalar product separately for each lattice site
void compute_square(const dev_vec &lattice, dev_vec &identity_scalar_product)
{
    thrust::transform(lattice.begin(), lattice.end(), identity_scalar_product.begin(), square());
}


void drift_computation(const dev_vec &lattice, dev_vec &drift, dev_vec &identity_scalar_product,
                       const dev_vec_vec_int& neighbour_indices, unsigned long long &rnd_offset,
                       const double kappa, const double lambda)
{
    thrust::fill(drift.begin(), drift.end(), 0.0);

    // Computes for each phi_i the drift term:
    //     -2.0 * kappa \sum_neighbours \phi_j + 2.0 * (1 - 2.0 * lambda) phi_i + 4.0 * lambda * phi_i^3,
    // where the sum runs over all neighbours of phi_i and performs a Langevin update.

    //[ Compute interaction term \sum_neighbours \phi_neighbour

    // Sum over neighbours for each site AND each component by looping over all neighbours
    for(size_t n = 0; n < neighbour_indices.size(); n++) {
        thrust::for_each(
                thrust::make_zip_iterator(thrust::make_tuple(
                        drift.begin(), thrust::make_permutation_iterator(lattice.begin(), neighbour_indices[n].begin()))),
                thrust::make_zip_iterator(thrust::make_tuple(
                        drift.end(), thrust::make_permutation_iterator(lattice.end(), neighbour_indices[n].end()))),
                drift_interaction()
        );
    }
    //]

    //[ Compute the scalar product \phi^2

    // Compute the inner product
    compute_square(lattice, identity_scalar_product);

    //[ Add remaining terms and finalize the drift computation
    thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(lattice.begin(), drift.begin(), identity_scalar_product.begin())),
                     thrust::make_zip_iterator(thrust::make_tuple(lattice.end(), drift.end(), identity_scalar_product.end())),
                     compute_drift(kappa, lambda));
    // Update offset for random number generation
    rnd_offset += 1;
}


double action_helper(const dev_vec &lattice, dev_vec& potential, dev_vec &identity_scalar_product,
                     const dev_vec_vec_int& neighbour_indices, const double kappa, const double lambda)
{
    // Computes the action of the scalar theory according to
    //     S = -2.0 * kappa \sum_<i, j> \phi_i \phi_j + \sum_i (1 - 2.0 * lambda) phi_i^2 + lambda * phi_i^4
    // where the sum runs over neighbour pairs phi_i (note that in the computation only the right hand side neighbours
    // need to be taken into account.

    thrust::fill(potential.begin(), potential.end(), 0.0);

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

    //[ Compute the scalar product \phi^2
    compute_square(lattice, identity_scalar_product);
    //]

    //[ Add remaining terms and compute sum

    // <-> potential = -2.0 * mp.kappa * potential + site * site + mp.lambda * pow(site * site - 1.0, 2.0);
    thrust::transform(potential.begin(), potential.end(), identity_scalar_product.begin(), potential.begin(), compute_action(kappa, lambda));
    return thrust::reduce(potential.begin(), potential.end());
    //]
}


// Accumulates all sites of the lattice
double accumulate(const dev_vec &lattice)
{
    return thrust::reduce(lattice.begin(), lattice.end());
}


// Second moment
double second_moment_helper(const dev_vec &lattice, dev_vec &identity_scalar_product)
{
    compute_square(lattice, identity_scalar_product);
    return thrust::reduce(identity_scalar_product.begin(), identity_scalar_product.end()) / lattice.size();
}


// Fourth moment
double fourth_moment_helper(const dev_vec &lattice, dev_vec &identity_scalar_product)
{
    compute_square(lattice, identity_scalar_product);
    return thrust::inner_product(identity_scalar_product.begin(), identity_scalar_product.end(), identity_scalar_product.begin(), 0.0f) / lattice.size();
}


//* Implementation of a langevin update *//

/* void update_step_helper(uint measure_interval, dev_vec &lattice, thrust::device_vector<curandState> &s, dev_vec_vec_int &neighbour_indices,
                        unsigned long long &rnd_offset, const double epsilon, const double kappa, const double lambda)
{
    for(uint i = 0; i < measure_interval; i++)
    {
        // Computes for each phi_i the drift term:
        //     -2.0 * kappa \sum_neighbours \phi_j + 2.0 * (1 - 2.0 * lambda) phi_i + 4.0 * lambda * phi_i^3,
        // where the sum runs over all neighbours of phi_i and performs a Langevin update.

        //[ Compute interaction term \sum_neighbours \phi_neighbour

        dev_vec interaction_term(lattice.size(), 0.0);

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
        dev_vec identity_scalar_product = compute_square(lattice);

        //[ Add remaining terms and update sites and add noise
        thrust::for_each(thrust::make_zip_iterator(thrust::make_tuple(s.begin(), lattice.begin(), interaction_term.begin(), identity_scalar_product.begin())),
                         thrust::make_zip_iterator(thrust::make_tuple(s.end(), lattice.end(), interaction_term.end(), identity_scalar_product.end())),
                         langevin_update(epsilon, kappa, lambda));
        // Update offset for random number generation
        rnd_offset += 1;
        //]
    }
} */