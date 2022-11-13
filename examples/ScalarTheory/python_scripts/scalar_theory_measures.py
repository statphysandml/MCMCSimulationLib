import numpy as np


from mcmctools.utils.lattice import get_neighbour_index, get_neighbour_index_in_line


def compute_scalar_theory_measures(data, measure_name, sim_params):
    if measure_name == "TwoPointCorrelation":
        return compute_two_point_correlation(data, sim_params)
    elif measure_name == "IthTwoPointCorrelation":
        return compute_ith_two_point_correlation(data, sim_params)


def compute_two_point_correlation(data, sim_params):
    dimensions = sim_params["systembase"]["dimensions"]
    dim_mul = np.cumprod([1] + dimensions)
    lattice_configs = data["Config"].values
    elem_per_site = 1
    n_sites = np.size(lattice_configs, axis=1)

    two_point_correlation = np.zeros(len(data))

    # Using translation symmetry
    for site in range(n_sites):
        # Using rotation symmetry
        for dim in range(len(dimensions)):
            neighbour = get_neighbour_index(n=site, dim=dim, direction=True, mu=0, dimensions=dimensions, dim_mul=dim_mul,
                                            elem_per_site=elem_per_site)
            two_point_correlation += lattice_configs[:, site] * lattice_configs[:, neighbour]

    data.insert(len(data.columns), "TwoPointCorrelation", two_point_correlation / (n_sites * len(dimensions)))

    return ["TwoPointCorrelation"], data


def compute_ith_two_point_correlation(data, sim_params):
    dimensions = sim_params["systembase"]["dimensions"]
    dim_mul = np.cumprod([1] + dimensions)
    lattice_configs = data["Config"].values
    elem_per_site = 1
    n_sites = np.size(lattice_configs, axis=1)

    new_measures = []

    for i in range(1, dimensions[0]):

        two_point_correlation = np.zeros(len(data))
        # Using translation symmetry
        for site in range(n_sites):
            # Using rotation symmetry
            for dim in range(len(dimensions)):
                neighbour = get_neighbour_index_in_line(n=site, n_next=i, dim=dim, direction=True, mu=0,
                                                        dimensions=dimensions, dim_mul=dim_mul, elem_per_site=elem_per_site)
                # Using symmetries in exchanging two components of the n-vector
                two_point_correlation += lattice_configs[:, site] * lattice_configs[:, neighbour]

        new_measures.append("TwoPointCorrelationIth" + str(i))
        data.insert(len(data.columns), "TwoPointCorrelationIth" + str(i), two_point_correlation / (n_sites * len(dimensions)))

    return new_measures, data
