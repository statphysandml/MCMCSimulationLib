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
    evaluation_module = EvaluationModule(sim_base_dir="../", rel_data_dir="data/{{ cookiecutter.project_name }}Simulation/",
                                         rel_results_dir="results/{{ cookiecutter.project_name }}Simulation/",
                                         running_parameter_kind=None,
                                         running_parameter="sigma",
                                         rp_values=linspace_rp_intervals(0.2, 1.0, 9))

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
    from custom_modules import compute_measures

    sim_params = evaluation_module.load_sim_params(identifier="expectation_value")

    # The "NormalizedRandomVariable" computes the sum for each sample and normalizes the resulting random variable
    # by the expected mean and standard deviation. The resulting random variable is expected to follow a standard
    # normal distribution.
    evaluation_module.compute_expectation_values(
        measures=["NormalizedRandomVariable"], error_type="statistical",  n_means_bootstrap=0,
        custom_measures_func=get_custom_measures_func(), custom_measures_args=json.dumps(sim_params),
        custom_load_data_func=None, custom_load_data_args=None
    )

    expectation_values = evaluation_module.expectation_values
    print(expectation_values)

    number_of_measurements = sim_params["execution_mode"]["number_of_measurements"]

    fig, ax = fma.newfig(1.0)
    # Since "Error" refers to the standard error, we expect that Std * sqrt(number_of_measurements) is equal to one for
    # all different values of sigma
    ax.plot(expectation_values["Estimate"]["Sigma"].values,
            expectation_values["Error"]["NormalizedRandomVariable"].values * np.sqrt(number_of_measurements))
    ax.plot([0.2, 1.0], [1.0, 1.0], "--", color="grey")
    ax.set_xlabel("Sigma")
    ax.set_ylabel("Measured standard deviation")
    plt.tight_layout()
    plt.show()

    """ Plot sampled Gaussian distributions """

    import pandas as pd

    data = evaluation_module.load_data(identifier="expectation_value")
    sim_params = evaluation_module.load_sim_params(identifier="expectation_value")
    samples = pd.concat([data["Config"], data["Mean"]], axis=1)

    from pystatplottools.distributions.marginal_distribution import MarginalDistribution
    marginal_distribution = MarginalDistribution(data=samples)

    range_min, range_max = marginal_distribution.extract_min_max_range_values(
        [0, 1, 2, "Mean"]
    )

    marginal_distribution.compute(
        axes_indices=[0, 1, 2, "Mean"],
        range_min=range_min,
        range_max=range_max,
        nbins=[16, 16, 16, 16],
        statistic='probability',
        bin_scales=['linear', 'linear', 'linear', 'linear']
    )

    linearized_marginal_distribution = marginal_distribution.linearize(
        order_by_bin=True,
        bin_alignment="center"
    )

    from pystatplottools.utils.bins_and_alignment import revert_align_bins
    sigmas = [0.3, 0.9]
    mus = ["mu = " + str(mu) for mu in sim_params["systembase_params"]["mu"]] + ["mean"] + ["normalized"]

    fig, axes = fma.newfig(1.8, nrows=1, ncols=2, ratio=1.0, figsize=(11, 4))
    for i, sigma in enumerate(sigmas):
        for j, col in enumerate([0, 1, 2, "Mean"]):
            binedges = revert_align_bins(
                data_range=linearized_marginal_distribution.loc[col]["bin"].values,
                bin_alignment="center",
                bin_scale="linear"
            )
            width = 0.9 * (binedges[1:] - binedges[:-1])

            axes[i].bar(
                x=linearized_marginal_distribution.loc[col]["bin"].values,
                height=linearized_marginal_distribution.loc[col]["{:.6f}".format(sigma)].values,
                width=width,
                label=mus[j]
            )
            axes[i].set_xlabel("x")
            axes[i].set_ylabel("P(x) for sigma = " + str(sigma))
            axes[i].legend()

    plt.tight_layout()
    plt.show()