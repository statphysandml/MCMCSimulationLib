import os
import sys

from util.json import load_json_params


def plot_site_distribution(files_dir, sim_root_dir="", rel_path="./"):
    # Load configs and data
    cwd = os.getcwd()

    config_path = os.getcwd() + "/configs/" + files_dir
    data_path = os.getcwd() + "/data/" + files_dir

    sim_params = load_json_params(config_path, "sim_params")
    execution_params = load_json_params(config_path, "plot_site_distribution_params")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        print("FilesDir:", sys.argv[1])  # , "SimRootDir:", sys.argv[2], "RelPath:", sys.argv[3])
        plot_site_distribution(sys.argv[1])  # , sys.argv[2], sys.argv[3])
    else:
        os.chdir("/home/lukas/LatticeModelImplementations/main_project")
        # expectation_value("IsingModel")  # , ".", True)
        # expectation_value("XYModelMetropolis")  # , ".", True)
        # expectation_value("XYModelHMC")  # , ".", True)
        # expectation_value("PolySiteModelComplexLangevin")  # , ".", True)
        # expectation_value("PolySiteModelCobridMonteCarlo")
        # load_expectation_value_results("XYModel")
        plot_site_distribution("CubicSiteModelComplexLangevin", ".", True)