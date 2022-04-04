import numpy as np


""" Due to the utilized MCMC algorithm, the data is not balanced in the ferromagnetic phase despite an expected
Z2 symmetry. The here provided functions can be used to enforce this based on randomly selected overall signs
of the configurations. """


def transform_data_set_balanced_data_set(sim_base_dir, rel_data_dir, rel_results_dir, running_parameter, rp_values):
    from mcmctools.mcmc.evaluation_module import EvaluationModule
    evaluation_module = EvaluationModule(sim_base_dir=sim_base_dir,
                                         rel_data_dir=rel_data_dir,
                                         rel_results_dir=rel_results_dir,
                                         running_parameter_kind=None,
                                         running_parameter=running_parameter,
                                         rp_values=rp_values)

    data = evaluation_module.load_data(identifier="expectation_value")
    data.loc[:, ("Config", slice(None))] = data.loc[:, ("Config", slice(None))].mul(
        2 * np.random.randint(0, 2, len(data)) - 1, axis=0)
    data.Mean = data.Config.values.mean(axis=1)

    def config_to_string(x):
        measure = ""
        for val in x:
            measure += f'{val:.6f}' + ", "
        return measure[:-2]

    configs = data.Config.apply(config_to_string, axis=1)
    del data["Config"]
    data = data.droplevel(1, axis=1)
    data.insert(loc=0, column='Config', value=configs)

    data.groupby(running_parameter).apply(lambda x: x.drop(columns=running_parameter.capitalize()).to_csv(
        sim_base_dir + "/" + rel_data_dir + "/expectation_value_" + running_parameter + "=" +
        f'{x[running_parameter.capitalize()].iloc[0]:.6f}' + ".dat", sep="\t", header=True, index=False
    ))


def update_transformed_expectation_values(sim_base_dir, rel_data_dir, rel_results_dir, running_parameter, rp_values):
    from mcmctools.mcmc.evaluation_module import EvaluationModule
    evaluation_module = EvaluationModule(sim_base_dir=sim_base_dir,
                                         rel_data_dir=rel_data_dir,
                                         rel_results_dir=results_dir,
                                         running_parameter_kind=None,
                                         running_parameter=running_parameter,
                                         rp_values=rp_values)

    sim_params = evaluation_module.load_sim_params(identifier="expectation_value")

    import json
    from custom_modules import compute_measures
    evaluation_module.compute_expectation_values(custom_measures_func=compute_measures,
        custom_measures_args=json.dumps(sim_params))


if __name__ == '__main__':
    # Ensure to be in the same directory as the file
    import os
    dir_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(dir_path)

    import sys
    # Append the path to custom_modules.py module to sys.path; required if measures are computed in Python (with the
    # post_measures parameter of the expectation value simulation mode) or, if a custom data loader is used.
    sys.path.append("../python_scripts/")

    from mcmctools.utils.utils import linspace_rp_intervals
    transform_data_set_balanced_data_set(sim_base_dir="../",
                                         rel_data_dir="data/ScalarTheorySimulation/",
                                         rel_results_dir="results/ScalarTheorySimulation/",
                                         running_parameter="kappa",
                                         rp_values=linspace_rp_intervals(0.22, 0.29, 8))
