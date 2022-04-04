from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=True)

import numpy as np
import json


""" Examples for loading and (re)evaluating results obtained from the generated MCMC configurations and measures """


if __name__ == '__main__':
    # Ensure to be in the same directory as the file
    import os
    dir_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(dir_path)

    import sys
    # Append the path to custom_modules.py module to sys.path; required if measures are computed in Python (with the
    # post_measures parameter of the expectation value simulation mode) or, if a custom data loader is used.
    sys.path.append("../python_scripts/")

    from mcmctools.mcmc.evaluation_module import EvaluationModule
    from mcmctools.utils.utils import linspace_rp_intervals
    evaluation_module = EvaluationModule(sim_base_dir="../", rel_data_dir="data/ScalarTheorySimulation/",
                                         rel_results_dir="results/ScalarTheorySimulation/",
                                         running_parameter_kind=None,
                                         running_parameter="kappa",
                                         rp_values=linspace_rp_intervals(0.22, 0.29, 8))

    """ Loading results """

    evaluation_module.load_equilibrium_times()
    print(evaluation_module.equilibrium_times)

    evaluation_module.load_correlation_times()
    print(evaluation_module.correlation_times)

    evaluation_module.load_expectation_values()
    print(evaluation_module.expectation_values)

    data = evaluation_module.load_data(identifier="expectation_value")
    sim_params = evaluation_module.load_sim_params(identifier="expectation_value")
    print(data)
    print(sim_params)

    """ Recomputing results """

    # Note that there is the possibility to recompute the equilibrium time with different values for
    # confidence_range, confidence_window
    evaluation_module.compute_equilibrium_time(fma=fma)
    print(evaluation_module.equilibrium_times)

    evaluation_module.compute_correlation_time(fma=fma)
    print(evaluation_module.correlation_times)

    # Note that there is the possibility to recompute expectation values with different values for
    # measures=None, error_type=None, n_means_bootstrap=None, examples are given below.
    evaluation_module.compute_expectation_values()
    print(evaluation_module.expectation_values)

    """ Computing additional expectation values """

    # Example for a computation of results - note that the custom_module.py file can be extended by any kind of measures
    # to allow for the computation of further expectation values.

    from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func
    # Alternatively, the custom_measure_func can be imported directly
    # from custom_modules import compute_measures


    sim_params = evaluation_module.load_sim_params(identifier="expectation_value")

    evaluation_module.compute_expectation_values(
        measures=["TwoPointCorrelation", "IthTwoPointCorrelation"], error_type="statistical",  n_means_bootstrap=0,
        custom_measures_func=get_custom_measures_func(), custom_measures_args=json.dumps(sim_params),
        custom_load_data_func=None, custom_load_data_args=None
    )

    two_point_correlations = np.stack(
        [evaluation_module.expectation_values["Estimate"]["TwoPointCorrelationIth" + str(i)].values for i in
         range(1, 32)]).transpose()

    connected_two_point_correlations = two_point_correlations - np.repeat(
        np.power(evaluation_module.expectation_values["Estimate"]["Mean"].values, 2.0), 31
    ).reshape(-1, 31)

    fig, ax = fma.newfig(1.0)
    for j in range(9):
        ax.plot([i for i in range(1, 32)], connected_two_point_correlations[j], label=evaluation_module.rp_values[j])
    ax.legend()
    plt.show()