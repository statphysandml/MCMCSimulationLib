import os


if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    mcmc_model = "IsingModelSimulation/"

    path = "../../data/" + mcmc_model
    root = "../../data/" + mcmc_model

    ''' Data generation with storage of a permament file '''

    data_generator_args = {
        # ConfigDataGenerator Args
        "data_type": "target_param",
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": path,
        "total_number_of_data_per_file": 10000,
        "identifier": "expectation_value",
        "running_parameter": "beta",
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    # Prepare in memory dataset
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import prepare_in_memory_dataset
    from mcmctools.pytorch.data_generation.datagenerationroutines import data_generator_factory

    prepare_in_memory_dataset(
        root=root,
        batch_size=89,
        data_generator_args=data_generator_args,
        data_generator_name="BatchConfigDataGenerator",
        data_generator_factory=data_generator_factory
    )

    # Load in memory dataset
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_memory_dataset
    data_loader = load_in_memory_dataset(
        root=root, batch_size=89, data_generator_factory=data_generator_factory, slices=None, shuffle=True,
        num_workers=0, rebuild=False
        # sample_data_generator_name="ConfigDataGenerator"  # optional: for a generation of new samples
    )

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))
