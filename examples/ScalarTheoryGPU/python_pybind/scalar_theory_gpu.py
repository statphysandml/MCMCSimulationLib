from scalartheorygpusimulation import ScalarTheoryGPUEquilibriumTime
from scalartheorygpusimulation import ScalarTheoryGPUCorrelationTime
from scalartheorygpusimulation import ScalarTheoryGPUExpectationValue


from mcmc.mcmc_system import MCMCSystem


class ScalarTheoryGPU(MCMCSystem):
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSim = ScalarTheoryGPUEquilibriumTime
    CorrTimeSim = ScalarTheoryGPUCorrelationTime
    ExpValSim = ScalarTheoryGPUExpectationValue

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
        from scalartheorygpusimulation import ScalarTheoryGPUSystem
        self._mcmc_system = ScalarTheoryGPUSystem(
            kappa=self.kappa, lambd=self.lambd, dimensions=self.dimensions,
            dt=self.dt, n=self.n, m=self.m)
        self._mcmc_system.set_measures(measures=self.measure_names)

    def initialize(self, starting_mode):
        # Required for mcmc simulation
        self.initialize_mcmc_system()
        self._mcmc_system.init(starting_mode=starting_mode)