import os
import time
import numpy as np


""" Same as in pytorch_in_memory_dataset but with pytorch_geometric """


if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Path to data
    rel_data_dir = "../data/ScalarTheorySimulation/"
    # Path to where to store
    rel_gen_data_path = "../data/ScalarTheorySimulation/pytorch_geometric_in_memory_dataset/"

    ''' Data generation with storage of a permanent file '''

    data_generator_args = {
        # GraphConfigDataGenerator Args
        "dimensions": [4, 4],
        "complex_config": False,
        # Args for ConfigurationLoader
        "path": os.path.abspath(os.path.dirname(os.path.realpath(__file__)) + "/" + rel_data_dir),
        "total_number_of_data_per_file": 1000,
        "identifier": "expectation_value",
        "running_parameter": "kappa",
        "chunksize": 400  # If no chunksize is given, all data is loaded at once
    }

    # Prepare in memory dataset
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import prepare_in_memory_dataset
    from mcmctools.pytorch.data_generation.datagenerationroutines import data_generator_factory

    prepare_in_memory_dataset(
        root=rel_gen_data_path,
        batch_size=128,
        data_generator_args=data_generator_args,
        data_generator_name="BatchGraphDataGenerator",
        data_generator_factory=data_generator_factory
    )

    # Load in memory dataset
    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_memory_dataset

    dataset = load_in_memory_dataset(
        root=rel_gen_data_path,
        data_generator_factory=data_generator_factory,
        rebuild=False,
        dataset_type="geometric"
    )

    from pystatplottools.pytorch_data_generation.data_generation.datagenerationroutines import load_in_memory_data_loader
    training_data_loader = load_in_memory_data_loader(dataset=dataset, batch_size=128, slices=(0, 3000), shuffle=True,
                                                      num_workers=0)
    test_data_loader = load_in_memory_data_loader(dataset=dataset, batch_size=128, slices=(3000, 4000), shuffle=False,
                                                  num_workers=0)

    t = time.time()

    # Load training data
    for batch_idx, batch in enumerate(training_data_loader):
        data = batch
        print(batch_idx, len(data.y))
        if batch_idx == 0: # Useful for verifying the shuffle parameter of the data loader
            print("Training", data.y.cpu().numpy().flatten())

    # Load training data - Second epoch
    for batch_idx, batch in enumerate(training_data_loader):
        data = batch
        print(batch_idx, len(data.y))
        if batch_idx == 0:
            print("Training", data.y.cpu().numpy().flatten())

    elapsed_time_from_file = np.round_(time.time() - t, 3)

    # Load test data
    for batch_idx, batch in enumerate(test_data_loader):
        data = batch
        print(batch_idx, len(data.y))
        if batch_idx == 0: # Useful for verifying the shuffle parameter of the data loader
            print("Test", data.y.cpu().numpy().flatten())

    # Load test data - Second epoch
    for batch_idx, batch in enumerate(test_data_loader):
        data = batch
        print(batch_idx, len(data.y))
        if batch_idx == 0: # Useful for verifying the shuffle parameter of the data loader
            print("Test", data.y.cpu().numpy().flatten())

    print("From File (GeometricDataLoader):", elapsed_time_from_file, " sec.")
