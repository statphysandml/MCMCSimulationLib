import os


from mcmctools.loading.loading import ConfigurationLoader


if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    configuration_loader_args = {
        "path": "../../data/IsingModelSimulation/",
        "total_number_of_data_per_file": 100,
        "identifier": "expectation_value",
        "running_parameter": "beta",
        "chunksize": 21,
        "drop_last": True
    }

    loader = ConfigurationLoader(**configuration_loader_args)

    for i in range(6):
        data = loader.get_next_chunk_collection(resample=False)
        print(i)

    data, filenames = ConfigurationLoader.load_all_configurations(
        path="../../data/IsingModelSimulation/",
        identifier="expectation_value",
        running_parameter="beta")
    from mcmctools.utils.json import load_configs
    sim_params, execution_params, running_parameter = load_configs(target_name="IsingModelSimulation", mode="expectation_value", sim_base_dir="../../")

