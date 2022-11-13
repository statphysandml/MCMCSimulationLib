import numpy as np


from mcmctools.utils.lattice import get_neighbour_index


def compute_{{ cookiecutter.project_slug }}_measures(data, measure_name, sim_params):
    if measure_name == "NormalizedRandomVariable":
        return compute_normalized_random_variable(data, sim_params)


def compute_normalized_random_variable(data, sim_params):
    mus = sim_params["systembase"]["mu"]

    # Compute expected mu and sigma for the sum of random variables
    total_mu = np.sum(mus)
    total_sigma = np.sqrt(len(mus) * data["Sigma"] ** 2)
    
    # Compute sum of random variables and rescale back to standard normal distribution
    normalized_samples = (data["Config"].sum(axis=1) - total_mu) / total_sigma

    # Insert results in to the existing data frame
    data.insert(len(data.columns), "NormalizedRandomVariable", normalized_samples)
    return ["NormalizedRandomVariable"], data
