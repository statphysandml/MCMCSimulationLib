#ifndef MCMCSIMULATIONLIB_ISING_MODEL_HPP
#define MCMCSIMULATIONLIB_ISING_MODEL_HPP


#include <mcmc_simulation/header.hpp>


// Example implementation of the Ising model


class IsingModel : public mcmc::simulation::MeasureSystemBase<IsingModel>
{
public:
    explicit IsingModel(const json params):
        MeasureSystemBase(params),
        // Configuration parameters
        beta(get_entry<double>("beta", 0.4)),
        J(get_entry<double>("J", 1.0)),
        h(get_entry<double>("h", 0.0)),
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

        rnd_lattice_site = std::uniform_int_distribution<uint>(0, get_size());
    }

    IsingModel(double beta_=0.4, double J_=1.0, double h_=0.0, std::vector<int> dimensions_={4, 4}) : IsingModel(json{
            {"beta", beta_},
            {"J", J_},
            {"h", h_},
            {"dimensions", dimensions_}
    })
    {}

    void initialize(std::string starting_mode)
    {
        generate_measures(measures);

        std::uniform_int_distribution<uint> uniint(0, 1);
        lattice = std::vector<int> (get_size(), 1.0);
        if(starting_mode == "hot")
        {
            for(auto &site : lattice)
                site = 2 * uniint(mcmc::util::gen) - 1;
        }
    }

    // Metropolis update step
    void update_step(uint measure_interval=1)
    {
        auto random_site_index = rnd_lattice_site(mcmc::util::gen);

        // Flip spin
        auto proposed_state = -1 * lattice[random_site_index];

        auto current_energy = local_energy(random_site_index, lattice[random_site_index]);
        auto proposal_energy = local_energy(random_site_index, proposed_state);

        if(rand(mcmc::util::gen) < std::min(1.0, std::exp(-1.0 * (proposal_energy - current_energy))))
            lattice[random_site_index] = proposed_state;
        // else
        //     reject
    }

    uint16_t get_size() const
    {
        return n_sites;
    }

    auto at(int i) const
    {
        return lattice[i];
    }

    auto& at(int i)
    {
        return lattice[i];
    }

    auto get_system_representation() const
    {
        return lattice;
    }

    auto& get_system_representation()
    {
        return lattice;
    }

    double local_energy(uint site_index, int site_value)
    {
        double energy = 0;
        for(uint d = 0; d < dimensions.size(); d++)
        {
            energy += lattice[neigh_dir(site_index, d, true)];
            energy += lattice[neigh_dir(site_index, d, false)];
        }
        return  -1.0 * beta * site_value * (J * energy + h); // 0.5
    }

private:
    double beta;
    double J;
    double h;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)

    std::vector<int> lattice;

    std::uniform_real_distribution<double> rand;
    std::uniform_int_distribution<uint> rnd_lattice_site;

    //site, moving dimension, direction
    int neigh_dir(int i, int d, bool dir) const {
        if(dir)
            return i-i%(dim_mul[d]*dimensions[d])+(i+dim_mul[d])%(dim_mul[d]*dimensions[d]);
        else
            return i-i%(dim_mul[d]*dimensions[d])+(i-dim_mul[d]+dim_mul[d]*dimensions[d])%(dim_mul[d]*dimensions[d]);
    }
};

#endif //MCMCSIMULATIONLIB_ISING_MODEL_HPP
