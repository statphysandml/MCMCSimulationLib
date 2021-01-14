import os

from mcmctools.modes.correlation_time import correlation_time

if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Navigate to examples/ directory as project root directory
    os.chdir("../../")
    # Generates plots of the correlation time in examples/results/IsingModelSimulation and
    # writes the correlation times in to the file correlation_time_results.json in the same folder
    # Also used by the C++ MCMCSimulationLib
    correlation_time(files_dir="IsingModelSimulation")

    from pystatplottools.utils.utils import load_json
    correlation_times = load_json("./results/IsingModelSimulation/correlation_time_results.json")
    print(correlation_times)
