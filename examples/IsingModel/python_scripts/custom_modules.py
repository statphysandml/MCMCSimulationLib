""" If provided, the two functions are imported and passed to the routines of the MCMCEValuationLib used for evaluating the equilibrium time, correlation time and expectation values.

This is implemented by the MCMCEvaluationLib/mcmctools/loading/custom_function_support.py file which tries to load the functions from the custom_modules.py module and by respective function calls in the .hpp files
in the MCMCSimulationLib/include/modes/ directory. The path to the custom_modules.py file can be set with the help of the cmake variable ``PYTHON_SCRIPTS_PATH``. """


""" Input:
    data: pandas dataframe containing the MCMC configurations
    measure_name: Name of the measure
    custom_measures_args: Optional additional arguments; when called from C++ this refers to the serialized simulation parameters
"""
def compute_measures(data, measure_name, custom_measures_args):
    import json
    sim_params = json.loads(custom_measures_args)
    from ising_model_measures import compute_ising_model_measures
    return compute_ising_model_measures(data=data, measure_name=measure_name, sim_params=sim_params)


""" Uncomment to overwrite the default load_data function in MCMCEvaluationLib/mcmctools/loading/loading.py """

""" Input:
    rel_data_dir: rel_data_dir to sim_base_dir
    running_parameter: running_parameter
    identifier: string indicating the type of performed execution mode ("equilibrium_time", "correlationtime" or "expectation_value"), sim_base_dir: when called from C++, this refers to the project_root_dir
    rp_values: list of values for the running_parameter
    custom_load_data_args: Optional additional arguments; when called from C++ this refers to the serialized simulation parameters
"""
# def custom_load_data(rel_data_dir, identifier, running_parameter,
#         rp_values, sim_base_dir, custom_load_data_args):
#     pass
