import os


""" Examples for loading MCMC configurations as a PyTorch dataset and a respective PyTorch dataloader directly from
the stored MCMC configurations

We only recommend to use this if the dataset is to large to be loaded at once. Otherwise the generation of an
InMemoryDataset is much faster.

The file contains different possible approaches to load the data. """


if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Path to data
    rel_data_path = "../data/ScalarTheorySimulation/"

    ''' Normal DataLoader and ConfigDataGenerator with an InRealTimeDataset (without storing the data again somehwere
    else'''

    from mcmctools.pytorch.data_generation.configdatagenerator import ConfigDataGenerator

    data_generator_args = {
        # ConfigDataGenerator Args
        "data_type": "target_param",
        "labels": "running_parameter",
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/" + rel_data_path),
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",  # Can also be a list of other computed quantities..stored in the file
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    data_generator = ConfigDataGenerator(**data_generator_args)

    # Possible usage:
    # sample = data_generator.sampler()

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.datasets import InRealTimeDataset
    dataset = InRealTimeDataset(datagenerator=data_generator, n=len(data_generator))

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.dataloaders import DataLoader
    data_loader_params = {
        'batch_size': 120,
        'shuffle': True,
        'num_workers': 0
    }
    data_loader = DataLoader(dataset=dataset, **data_loader_params)

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    ''' BatchDataLoader and BatchConfigDataGenerator with an InRealTimeDataset '''

    # The difference to the ConfigDataGenerator is that batches are generated already by the dataloader
    # This leads to a performance boost since the underlying data frame is accessed via slicing
    # i.e: batch = self.data.iloc[i:i+batch_size].values instead of
    #    : batch = np.stack([self.data.iloc[j] for j in range(i, i+batch_size])

    from mcmctools.pytorch.data_generation.batchconfigdatagenerator import BatchConfigDataGenerator

    data_generator_args = {
        # ConfigDataGenerator Args
        "data_type": "target_param",
        "labels": "running_parameter",
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/" + rel_data_path),
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",  # Can also be a list of other computed quantities..stored in the file
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    data_generator = BatchConfigDataGenerator(
        # BatchConfigDataGenerator Args
        batch_size=120,
        **data_generator_args)

    # Possible usage:
    # sample = data_generator.sampler() # returns batch

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.datasets import InRealTimeDataset

    dataset = InRealTimeDataset(datagenerator=data_generator, n=len(data_generator))

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.dataloaders import BatchDataLoader

    data_loader_params = {
        # 'raw_samples': True,  # Returns the sample from the BatchDataGenerator as it is. Can be used if the BatchDataGenerator produces already the correct datatype for training
        'shuffle': True,  # Where used in dataset -> not at all, but by the Dataloader??
        'num_workers': 0}
    data_loader = BatchDataLoader(dataset=dataset, **data_loader_params)

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    ''' Same as above with using the load_in_real_time_dataset and load_in_real_time_data_loader function '''

    data_generator_args = {
        # ConfigDataGenerator Args
        "data_type": "target_param",
        "labels": "running_parameter",
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/" + rel_data_path),
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",  # Can also be a list of other computed quantities..stored in the file
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    data_loader_params = {
        # 'raw_samples': True,  # Returns the sample from the BatchDataGenerator as it is. Can be used if the BatchDataGenerator produces already the correct datatype for training
        'shuffle': True,  # Used correctly by the Dataloader??
        'num_workers': 0}

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_real_time_dataset
    from mcmctools.pytorch.data_generation.datagenerationroutines import data_generator_factory
    dataset = load_in_real_time_dataset(
        data_generator_args=data_generator_args,
        batch_size=120,
        data_generator_name="BatchConfigDataGenerator",
        data_generator_factory=data_generator_factory,
        seed=None,
        set_seed=True
    )

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_real_time_data_loader
    data_loader = load_in_real_time_data_loader(
        dataset=dataset,
        data_loader_params=data_loader_params,
        data_loader_name="BatchDataLoader"
    )

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data, target = batch
        print(batch_idx, len(data))

