import numpy as np


def compute_ising_model_measures(data, measure_name, sim_params):
    if measure_name == "Energy":
        return compute_ising_model_energy(data, sim_params)


def compute_ising_model_energy(data, sim_params):
    dimensions = sim_params["systembase_params"]["dimensions"]
    dim_mul = np.cumprod([1] + dimensions)
    lattice_configs = data["Config"].values
    elem_per_site = 1
    n_sites = np.size(lattice_configs, axis=1)

    beta = data["Beta"].values
    J = sim_params["systembase_params"]["J"]
    h = sim_params["systembase_params"]["h"]

    energies = np.zeros(len(data))

    for site in range(n_sites):
        neighbour_indices = np.zeros(2 * len(dimensions), dtype=np.int)
        for dim in range(len(dimensions)):
            neighbour_indices[2 * dim] = get_neighbour_index(n=site, dim=dim, direction=True, mu=0, dimensions=dimensions, dim_mul=dim_mul, elem_per_site=elem_per_site)
            neighbour_indices[2 * dim + 1] = get_neighbour_index(n=site, dim=dim, direction=False, mu=0, dimensions=dimensions, dim_mul=dim_mul, elem_per_site=elem_per_site)

        energies += beta * lattice_configs[:, site] * (J / 2.0 * np.sum(lattice_configs[:, neighbour_indices], axis=1) + h)

    data.insert(len(data.columns), "Energy", energies / (n_sites * elem_per_site))

    return ["Energy"], data


# site, moving dimension, direction, index of site representation
def get_neighbour_index(n, dim, direction, mu, dimensions, dim_mul, elem_per_site):
    if direction:
        return (n - n % (dim_mul[dim] * dimensions[dim]) +
                (n + dim_mul[dim]) % (dim_mul[dim] * dimensions[dim])) * elem_per_site + mu
    else:
        return (n - n % (dim_mul[dim] * dimensions[dim]) +
                (n - dim_mul[dim] + dim_mul[dim] * dimensions[dim]) % (dim_mul[dim] * dimensions[dim])) * elem_per_site + mu
