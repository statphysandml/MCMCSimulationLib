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

    evaluation_module = EvaluationModule(sim_base_dir="../", rel_data_dir="data/{{ cookiecutter.project_name }}Simulation/", rel_results_dir="results/{{ cookiecutter.project_name }}Simulation/",
                                         running_parameter_kind=None,
                                         running_parameter="sigma",
                                         rp_values=linspace_rp_intervals(0.5, 1.5, 9))
    # expectation_values = evaluation_module.compute_equilibrium_time(from_file=True, measure="SecondMoment", fma=fma)

    # evaluation_module.compute_equilibrium_time(from_file=True)

    data, sim_params = evaluation_module.load_data_from_file()

    from mcmctools.loading.custom_function_support import get_custom_measures_func, get_custom_load_data_func

    evaluation_module.compute_expectation_value(
        number_of_measurements=1000, measures=["Config", "Mean"], eval_error_type="statistical",  eval_n_means_bootstrap=0,
        rp_values=None, from_file=False, custom_measures_func=None, custom_measures_args=None, # custom_measures_func=get_custom_measures_func(), custom_measures_args=json.dumps(sim_params),
        custom_load_data_func=None, custom_load_data_args=None
    )