import numpy as np


""" Example for accessing and executing C++ functions of the ScalarTheory in Python.

Since the ScalarTheory class inherits from the MCMCSystem class, support of the C++ functions
is automatically taken care of """


if __name__ == "__main__":
    from scalartheory.scalar_theory import ScalarTheory
    scalar_theory = ScalarTheory(kappa=0.23, lambd=0.02, dimensions=[4, 4], dt=0.14, n=10, m=1.0)

    scalar_theory.initialize("hot")
    print("System size:", scalar_theory.size())

    scalar_theory.measure_names = ["Mean", "SecondMoment"]
    print("Measure name:", scalar_theory.measure_names)

    print("Measurements:", scalar_theory.measure(), "Lattice at site 5:", scalar_theory[5])
    scalar_theory.update(n_step=100)
    print("Measurements:", scalar_theory.measure(), "Lattice at site 5:", scalar_theory[5])

    print("Lattice:", scalar_theory.system_repr)
    scalar_theory.system_repr = np.zeros_like(scalar_theory.system_repr)
    print("Reset Lattice: ", scalar_theory.system_repr)
    print("Measurements:", scalar_theory.measure(), "Lattice at site 5:", scalar_theory[5])
