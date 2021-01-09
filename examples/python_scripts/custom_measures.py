

def compute_measures(data, measure_name, sim_params):
    from ising_model_measures import compute_ising_model_measures
    return compute_ising_model_measures(data=data, measure_name=measure_name, sim_params=sim_params)
