from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=True)

import numpy as np
import pandas as pd
import json

from mcmctools.utils.utils import linspace_rp_intervals
from mcmctools.mcmc.evaluation_module import EvaluationModule


if __name__ == '__main__':
    import sys
    sys.path.append("../python_scripts/")

    evaluation_module = EvaluationModule(sim_base_dir="../", rel_data_path="data/ScalarTheorySimulation/", rel_results_path="results/ScalarTheorySimulation/",
                                         running_parameter_kind=None,
                                         running_parameter="kappa",
                                         rp_values=linspace_rp_intervals(0.22, 0.3, 9))
    # expectation_values = evaluation_module.compute_equilibrium_time(from_file=True, measure="SecondMoment", fma=fma)

    # evaluation_module.compute_equilibrium_time(from_file=True)

    data, sim_params = evaluation_module.load_data_from_file()

    from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func

    evaluation_module.compute_expectation_value(
        number_of_measurements=1000, measures=["TwoPointCorrelation", "IthTwoPointCorrelation"], eval_error_type="statistical",  eval_n_means_bootstrap=0,
        rp_values=None, from_file=False, custom_measures_func=get_custom_measures_func(), custom_measures_args=json.dumps(sim_params),
        custom_load_data_func=None, custom_load_data_args=None
    )

    two_point_correlations = np.stack(
        [evaluation_module.expectation_values["Estimate"]["TwoPointCorrelationIth" + str(i)].values for i in
         range(1, 32)]).transpose()

    connected_two_point_correlations = two_point_correlations - np.repeat(np.power(evaluation_module.expectation_values["Estimate"]["Mean"].values, 2.0), 31).reshape(-1, 31)

    fig, ax = fma.newfig(1.0)

    for j in range(9):
        ax.plot([i for i in range(1, 32)], connected_two_point_correlations[j], label=evaluation_module.rp_values[j])

    # ax.set_yscale("log")
    ax.legend()
    plt.show()
#    fma.savefig()
    print(1)
