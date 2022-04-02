import numpy as np
import time


if __name__ == '__main__':
    # To ensure to run code from this file
    import os
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    ''' BatchDataLoader and BatchConfigDataGenerator with an InRealTimeDataset '''

    data_generator_args = {
        "dimensions": [4, 4],
        "path": "./data/Test/",
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",
        "chunksize": 400
    }

    from mcmctools.pytorch.data_generation.batchgraphdatagenerator import BatchGraphDataGenerator

    data_generator = BatchGraphDataGenerator(
        # BatchConfigDataGenerator Args
        batch_size=128,
        # ConfigDataGenerator Args
        # data_type="target_param",
        # DataGeneratoreBaseClass Args
        seed=None,
        set_seed=True,
        # ConfigurationLoader Args
        **data_generator_args)

    # Possible usage:
    # sample = data_generator.sampler()

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.datasets import InRealTimeDataset

    dataset = InRealTimeDataset(datagenerator=data_generator, n=len(data_generator))

    from pystatplottools.pytorch_data_generation.pytorch_data_utils.dataloaders import BatchDataLoader

    data_loader_params = {
        # 'raw_samples': True,  # Returns the sample from the BatchDataGenerator as it is. Can be used if the BatchDataGenerator produces already the correct datatype for training
        'shuffle': True,  # Where used in dataset -> not at all, but by the Dataloader??
        'num_workers': 0}
    data_loader = BatchDataLoader(dataset=dataset, **data_loader_params)

    t = time.time()

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data = batch
        print(batch_idx, len(data.y))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data = batch
        print(batch_idx, len(data.y))

    elapsed_time = np.round_(time.time() - t, 3)

    ''' Same as above with using the load_in_real_time_data_loader function '''

    data_generator_args = {
        # Args for BatchGraphDataGenerator
        "dimensions": [4, 4],
        # Args for ConfigurationLoader
        "path": "./data/Test/",
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",
        "chunksize": 400 # If no chunksize is given, all data is loaded at once
    }

    data_loader_params = {
        # 'raw_samples': True,  # Returns the sample from the BatchDataGenerator as it is. Can be used if the BatchDataGenerator produces already the correct datatype for training
        'shuffle': True,  # Used correctly by the Dataloader??
        'num_workers': 0}

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import \
        load_in_real_time_dataset
    from mcmctools.pytorch.data_generation.datagenerationroutines import data_generator_factory
    dataset = load_in_real_time_dataset(
        data_generator_args=data_generator_args,
        batch_size=128,
        data_generator_name="BatchGraphDataGenerator",
        data_generator_factory=data_generator_factory,
        seed=None,
        set_seed=True
    )

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import \
        load_in_real_time_data_loader
    data_loader = load_in_real_time_data_loader(
        dataset=dataset,
        data_loader_params=data_loader_params,
        data_loader_name="BatchDataLoader"
    )

    t = time.time()

    # Load training data
    for batch_idx, batch in enumerate(data_loader):
        data = batch
        print(batch_idx, len(data.y))

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(data_loader):
        data = batch
        print(batch_idx, len(data.y))

    elapsed_time2 = np.round_(time.time() - t, 3)

    print("Real Time (BatchDataLoader):", elapsed_time, " sec. / ", elapsed_time2,
          " sec.")

