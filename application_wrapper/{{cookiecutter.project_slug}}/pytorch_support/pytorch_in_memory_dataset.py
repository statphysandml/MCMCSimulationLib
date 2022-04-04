import os


""" Example for loading and storing MCMC configurations as a PyTorch dataset.

After processing all MCMC configurations,
the data is stored as .pt file and can be loaded afterwards as a standard PyTorch dataset. """


if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Path to data
    rel_data_dir = "../data/{{ cookiecutter.project_name }}Simulation/"
    # Path to where to store
    rel_gen_data_path = "../data/{{ cookiecutter.project_name }}Simulation/pytorch_in_memory_dataset/"

    ''' Data generation with storage of a permament file '''

    data_generator_args = {
        # ConfigDataGenerator Args
        "data_type": "target_param",
        "labels": ["Sigma", "Mean"],
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/" + rel_data_dir),
        "total_number_of_data_per_file": 100000,
        "identifier": "expectation_value",
        "running_parameter": "sigma",  # Can also be a list of other computed quantities..stored in the file
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    # Prepare in memory dataset
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import prepare_in_memory_dataset
    from mcmctools.pytorch.data_generation.datagenerationroutines import data_generator_factory

    # Prepare in memory dataset - write a config.json file providing all necessary information for generating the
    # dataset
    prepare_in_memory_dataset(
        root=rel_gen_data_path,
        batch_size=128,
        data_generator_args=data_generator_args,
        data_generator_name="BatchConfigDataGenerator",
        data_generator_factory=data_generator_factory
    )

    # Load the in memory dataset (when this function is called the first time, the dataset is generated, otherwise,
    # only loaded)
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_memory_dataset
    dataset = load_in_memory_dataset(
        root=rel_gen_data_path,
        data_generator_factory=data_generator_factory,
        rebuild=False,
        sample_data_generator_name=None  # "ConfigDataGenerator"  # optional: for a generation of new samples
    )

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_memory_data_loader
    training_data_loader = load_in_memory_data_loader(dataset=dataset, batch_size=128, slices=(0, 600000), shuffle=True,
                                                      num_workers=0)

    # Load training data
    for batch_idx, batch in enumerate(training_data_loader):
        data, target = batch
        print(batch_idx, len(data))
        if batch_idx == 0:  # Useful for verifying the shuffle parameter of the data loader
            print(data)

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(training_data_loader):
        data, target = batch
        print(batch_idx, len(data))
        if batch_idx == 0:
            print(data)


