
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
    from scalar_theory_measures import compute_scalar_theory_measures
    return compute_scalar_theory_measures(data=data, measure_name=measure_name, sim_params=sim_params)


""" Uncomment to overwrite the default load_data function in MCMCEvaluationLib/mcmctools/loading/loading.py """
# def custom_load_data(rel_data_dir, identifier, running_parameter,
#         rp_values, sim_base_dir, custom_load_data_args):
#     pass
