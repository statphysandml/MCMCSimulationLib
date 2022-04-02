from scalartheorysimulation import ScalarTheoryEquilibriumTime
from scalartheorysimulation import ScalarTheoryCorrelationTime
from scalartheorysimulation import ScalarTheoryExpectationValue


from mcmc.mcmc_system import MCMCSystem


class ScalarTheory(MCMCSystem):
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSim = ScalarTheoryEquilibriumTime
    CorrTimeSim = ScalarTheoryCorrelationTime
    ExpValSim = ScalarTheoryExpectationValue

    def __init__(self, kappa, lambd, dimensions, dt, n, m, **kwargs):
        super().__init__(**kwargs)
        # Required for mode simulation and mcmc simulation
        self.kappa = kappa
        self.lambd = lambd
        self.dimensions = dimensions
        self.dt = dt
        self.n = n
        self.m = m

    def initialize_mcmc_system(self):
        # Required for mode simulation and mcmc simulation
        from scalartheorysimulation import ScalarTheorySystem
        self._mcmc_system = ScalarTheorySystem(
            kappa=self.kappa, lambd=self.lambd, dimensions=self.dimensions,
            dt=self.dt, n=self.n, m=self.m)
