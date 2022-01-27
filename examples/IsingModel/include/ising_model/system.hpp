//
// Created by lukas on 01.12.20.
//

#ifndef MCMCSIMULATIONLIB_ISING_MODEL_HPP
#define MCMCSIMULATIONLIB_ISING_MODEL_HPP


#include <mcmc_simulation/header.hpp>


// Example implementation of the Ising model


class System;

class SystemParameters : public mcmc::simulation::SystemBaseParameters {
public:
    explicit SystemParameters(const json params):
            SystemBaseParameters(params),
            beta(get_entry<double>("beta", 0.4)),
            J(get_entry<double>("J", 1.0)),
            h(get_entry<double>("h", 0.0)),
            dimensions(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4}))

    {
        n_sites = 1;
        dim_mul.push_back(n_sites);
        for(auto dim: dimensions) {
            n_sites *= dim;
            dim_mul.push_back(n_sites);
        }
    }

    SystemParameters(double beta_, double J_, double h_, std::vector<int> dimensions_) : SystemParameters(json{
            {"beta", beta_},
            {"J", J_},
            {"h", h_},
            {"dimensions", dimensions_}
    })
    {}

    std::unique_ptr<System> generate() { return std::make_unique<System>(*this); }

private:
    friend class System;

    const double beta;
    const double J;
    const double h;

    uint16_t n_sites; // Total number of sites
    std::vector<int> dimensions; // Different dimensions
    std::vector<int> dim_mul; // Accumulated different dimensions (by product)
};


class System : public mcmc::simulation::MeasureSystemBase<System>
{
public:
    explicit System(const SystemParameters &imsp_) :
        imsp(imsp_),
        rand(std::uniform_real_distribution<double> (0,1)),
        rnd_lattice_site(std::uniform_int_distribution<uint>(0, get_size()))
    {}

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

    void initialize(std::string starting_mode)
    {
        generate_measures(imsp.measures);
        
        std::uniform_int_distribution<uint> uniint(0, 1);
        lattice = std::vector<int> (get_size(), 1.0);
        if(starting_mode == "hot")
        {
            for(auto &site : lattice)
                site = 2 * uniint(mcmc::util::gen) - 1;
        }
    }

    uint16_t get_size() const
    {
        return imsp.n_sites;
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
        for(uint d = 0; d < imsp.dimensions.size(); d++)
        {
            energy += lattice[neigh_dir(site_index, d, true)];
            energy += lattice[neigh_dir(site_index, d, false)];
        }
        return  -1.0 * imsp.beta * site_value * (imsp.J * energy + imsp.h); // 0.5
    }

private:
    std::vector<int> lattice;

    const SystemParameters &imsp;

    std::uniform_real_distribution<double> rand;
    std::uniform_int_distribution<uint> rnd_lattice_site;

    //site, moving dimension, direction
    int neigh_dir(int n, int d, bool dir) const {
        if(dir)
            return n-n%(imsp.dim_mul[d]*imsp.dimensions[d])+(n+imsp.dim_mul[d])%(imsp.dim_mul[d]*imsp.dimensions[d]);
        else
            return n-n%(imsp.dim_mul[d]*imsp.dimensions[d])+(n-imsp.dim_mul[d]+imsp.dim_mul[d]*imsp.dimensions[d])%(imsp.dim_mul[d]*imsp.dimensions[d]);
    }
};

#endif //MCMCSIMULATIONLIB_ISING_MODEL_HPP
