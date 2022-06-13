#ifndef MCMCSIMULATIONLIB_ISING_MODEL_HPP
#define MCMCSIMULATIONLIB_ISING_MODEL_HPP


#include <mcmc/mcmc_simulation/header.hpp>


// Example implementation of the Ising model


class IsingModel : public mcmc::simulation::MeasureSystemBase<IsingModel>
{
public:
    explicit IsingModel(const json params):
        MeasureSystemBase(params),
        // Configuration parameters
        beta_(get_entry<double>("beta", 0.4)),
        J_(get_entry<double>("J", 1.0)),
        h_(get_entry<double>("h", 0.0)),
        dimensions_(get_entry< std::vector<int> >("dimensions", std::vector<int> {4, 4})),

        // Further member variables
        rand_(std::uniform_real_distribution<double> (0,1))
    {
        n_sites_ = 1;
        dim_mul_.push_back(n_sites_);
        for(auto dim: dimensions_) {
            n_sites_ *= dim;
            dim_mul_.push_back(n_sites_);
        }

        rnd_lattice_site_ = std::uniform_int_distribution<uint>(0, get_size());
    }

    IsingModel(double beta=0.4, double J=1.0, double h=0.0, std::vector<int> dimensions={4, 4}) : IsingModel(json{
            {"beta", beta},
            {"J", J},
            {"h", h},
            {"dimensions", dimensions}
    })
    {}

    void initialize(std::string starting_mode)
    {
        // It is important to generate the predefined measures based on the virtual method of the base class - Note that this method can,
        // of course also be overloaded.
        generate_measures(measures_);

        std::uniform_int_distribution<uint> uniint(0, 1);
        lattice_ = std::vector<int> (get_size(), 1.0);
        if(starting_mode == "hot")
        {
            for(auto &site : lattice_)
                site = 2 * uniint(mcmc::util::g_gen) - 1;
        }
    }

    // Metropolis update step
    void update_step(uint measure_interval=1)
    {
        auto random_site_index = rnd_lattice_site_(mcmc::util::g_gen);

        // Flip spin
        auto proposed_state = -1 * lattice_[random_site_index];

        auto current_energy = local_energy(random_site_index, lattice_[random_site_index]);
        auto proposal_energy = local_energy(random_site_index, proposed_state);

        if(rand_(mcmc::util::g_gen) < std::min(1.0, std::exp(-1.0 * (proposal_energy - current_energy))))
            lattice_[random_site_index] = proposed_state;
        // else
        //     reject
    }

    uint16_t get_size() const
    {
        return n_sites_;
    }

    auto at(int i) const
    {
        return lattice_[i];
    }

    auto& at(int i)
    {
        return lattice_[i];
    }

    auto get_system_representation() const
    {
        return lattice_;
    }

    auto& get_system_representation()
    {
        return lattice_;
    }

    double local_energy(uint site_index, int site_value)
    {
        double energy = 0;
        for(uint d = 0; d < dimensions_.size(); d++)
        {
            energy += lattice_[neigh_dir(site_index, d, true)];
            energy += lattice_[neigh_dir(site_index, d, false)];
        }
        return  -1.0 * beta_ * site_value * (J_ * energy + h_); // 0.5
    }

private:
    double beta_;
    double J_;
    double h_;

    uint16_t n_sites_; // Total number of sites
    std::vector<int> dimensions_; // Different dimensions
    std::vector<int> dim_mul_; // Accumulated different dimensions (by product)

    std::vector<int> lattice_;

    std::uniform_real_distribution<double> rand_;
    std::uniform_int_distribution<uint> rnd_lattice_site_;

    //site, moving dimension, direction
    int neigh_dir(int i, int d, bool dir) const {
        if(dir)
            return i-i%(dim_mul_[d]*dimensions_[d])+(i+dim_mul_[d])%(dim_mul_[d]*dimensions_[d]);
        else
            return i-i%(dim_mul_[d]*dimensions_[d])+(i-dim_mul_[d]+dim_mul_[d]*dimensions_[d])%(dim_mul_[d]*dimensions_[d]);
    }
};

#endif //MCMCSIMULATIONLIB_ISING_MODEL_HPP
