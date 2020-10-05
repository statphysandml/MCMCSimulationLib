import sys
import os
import copy
import numpy as np
import pandas as pd


from pystatplottools.ppd_distributions.expectation_value import ExpectationValue

sys.path.append("/home/lukas/MCMCSimulationLib/python_scripts/")
sys.path.append("/home/lukas/MCMCSimulationLib/python_scripts/util/")

from util.json import load_configs, load_data


def compute_measure_over_config(data, measure_name):
    if measure_name == "1stMoment":
        return compute_nth_moment(data, 1, measure_name)
    elif measure_name == "2ndMoment":
        return compute_nth_moment(data, 2, measure_name)
    elif measure_name == "3rdMoment":
        return compute_nth_moment(data, 3, measure_name)
    elif measure_name == "AbsMean":
        return compute_abs_mean(data)
    elif measure_name == "Energy":
        data.insert(len(data.columns), "Energy", data["Config"].apply(lambda x: np.abs(np.mean(x))))
        return ["Energy"]
    elif measure_name == "TwoPointCorrelator":
        return compute_two_point_correlator(data, key="Config")
    else:
        assert False, "Unknown measure"


def compute_nth_moment(data, n, measure_name):
    new_measures = []
    for col in data.columns:
        if col == "Config":
            pass
        # Only applicaple for one-dimensional data (site models)
        if col in ["Mean", "StateReal", "StateImag"]:
            data.insert(len(data.columns), measure_name + col, data[col].apply(lambda x: np.power(x, n)))
            new_measures.append(measure_name + col)
        # For one-dimensional data (site models)
        if col in ["StateReal"]:
            res = (data.StateReal + data.StateImag * 1j).apply(lambda x: np.power(x, n))
            data.insert(len(data.columns), "Complex" + measure_name + "StateReal", res.apply(lambda x: x.real))
            data.insert(len(data.columns), "Complex" + measure_name + "StateImag", res.apply(lambda x: x.imag))
            new_measures.append("Complex" + measure_name + "StateReal")
            new_measures.append("Complex" + measure_name + "StateImag")
    return new_measures


def compute_two_point_correlator(data, key="Config"):
    new_measures = []
    for i in range(len(data[key].iloc[0])):
        data.insert(len(data.columns), "TwoPCDelt" + str(i), data[key].apply(lambda x: np.multiply(x, np.roll(x, i)).sum()))
        new_measures.append("TwoPCDelt" + str(i))
    return new_measures


def compute_abs_mean(data):
    data.insert(len(data.columns), "AbsMean", data["Config"].apply(lambda x: np.abs(np.mean(x))))
    return ["AbsMean"]


def compute_measures_over_config(data, measures):
    effective_measures = copy.deepcopy(measures)
    for measure in measures:
        if measure not in data.columns:
            new_measures = compute_measure_over_config(data=data, measure_name=measure)
            if len(new_measures) > 0:
                effective_measures += new_measures
                effective_measures.remove(measure)
    return effective_measures


def expectation_value(files_dir, sim_root_dir="", rel_path="./"):
    # Load configs and data
    cwd = os.getcwd()

    sim_params, execution_params, running_parameter = load_configs(files_dir=files_dir, mode="expectation_value")
    data, filenames = load_data(files_dir=files_dir, running_parameter=running_parameter, identifier="expectation_value")

    # Compute measures based on the given configurations that have not been computed during the simulation
    post_measures = execution_params["post_measures"]
    if post_measures is not None:
        post_measures = compute_measures_over_config(data=data, measures=post_measures)


    # Compute the expectation values and the bootstrap error
    ep = ExpectationValue(data=data)

    expectation_value_measures = []
    if sim_params["systembase_params"]["measures"] is not None:
        expectation_value_measures += sim_params["systembase_params"]["measures"]
    if post_measures is not None:
        expectation_value_measures += post_measures

    black_expectation_value_list = ["Config"]
    expectation_value_measures = [
        exp_value for exp_value in expectation_value_measures if exp_value not in black_expectation_value_list]

    ep.compute_expectation_value(columns=expectation_value_measures,
                                 exp_values=['mean', 'max', 'min', 'secondMoment', 'fourthMoment'])
    expectation_values = ep.expectation_values

    if "n_means_bootstrap" in execution_params.keys() and execution_params["n_means_bootstrap"] != 0:
        ep.compute_error_with_bootstrap(n_means_boostrap=execution_params["n_means_bootstrap"],
                                        number_of_measurements=execution_params["number_of_measurements"],
                                        columns=expectation_value_measures,
                                        exp_values=['mean', 'secondMoment', 'fourthMoment'],
                                        running_parameter=running_parameter)
        bootstrap_errors = ep.bootstrap_errors
        results = pd.concat([expectation_values, bootstrap_errors], keys=["ExpectationValue", "BootstrapError"], axis=1)
    else:
        results = pd.concat([expectation_values], keys=["ExpectationValue", "BootstrapError"], axis=1)

    results = results.transpose()
    results.index.names = ["Measure", "Observable", "Statistics"]
    results = results.sort_index(level=1).reset_index()
    results = results.transpose()

    if not os.path.isdir(os.getcwd() + "/results/" + files_dir):
        os.makedirs(os.getcwd() + "/results/" + files_dir)

    results.to_json(os.getcwd() + "/results/" + files_dir + "/expectation_value_results.json")

    os.chdir(cwd)


def load_expectation_value_results(files_dir):
    results = pd.read_json(os.getcwd() + "/results/" + files_dir + "/expectation_value_results.json")
    results = results.transpose()
    results = results.set_index(["Measure", "Observable", "Statistics"])
    # results.index.names = ["Measure", "Observable", "Statistics"]
    results = results.transpose()
    return results


if __name__ == '__main__':
    if len(sys.argv) > 1:
        print("FilesDir:", sys.argv[1])  # , "SimRootDir:", sys.argv[2], "RelPath:", sys.argv[3])
        expectation_value(sys.argv[1])  # , sys.argv[2], sys.argv[3])
    else:
        # os.chdir("/home/lukas/LatticeModelImplementations/examples")
        os.chdir("/home/lukas/BellInequalityLangevin/Paper_ComplexMonteCarlo/Code")
        # expectation_value("IsingModelMetropolis")  # , ".", True)
        expectation_value("ExpectationValueComplexPolynomialModelCobridMonteCarloA", ".", True)