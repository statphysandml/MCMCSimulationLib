import os

from mcmctools.modes.expectation_value import expectation_value, load_expectation_value_results

if __name__ == '__main__':
    # To ensure to run code from this file
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Navigate to examples/ directory as project root directory
    os.chdir("../../")

    # Add python_scripts path to sys.path to include it as a module which contains custom_measures.py
    import sys
    sys.path.append("./python_scripts/")

    # Also used by the C++ MCMCSimulationLib
    expectation_value(target_name="IsingModelSimulation")

    expectation_values = load_expectation_value_results(target_name="IsingModelSimulation")
    print(expectation_values)

