
# Imported in MCMCEvaluationLib/mcmctools/loading/custom_function_support and loaded in
# MCMCSimulationLib/include/execution/modes/equilibrium_time

""" Input:
    data: pandas Dataframe
    measure_name: ...
    custom_measures_args ...
"""
def compute_measures(data, measure_name, custom_measures_args):
    import json
    sim_params = json.loads(custom_measures_args)
    from ising_model_measures import compute_ising_model_measures
    return compute_ising_model_measures(data=data, measure_name=measure_name, sim_params=sim_params)


""" Uncomment to overwrite the default load_data function in MCMCEvaluationLib/mcmctools/loading/loading.py """
# def custom_load_data(rel_data_dir, running_parameter, identifier,
#         sim_base_dir, rp_values, custom_load_data_args):
#     pass
