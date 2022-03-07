from scalartheorygpusimulation import ScalarTheoryGPUEquilibriumTimeParameters
from scalartheorygpusimulation import ScalarTheoryGPUEquilibriumTime
from scalartheorygpusimulation import ScalarTheoryGPUCorrelationTimeParameters
from scalartheorygpusimulation import ScalarTheoryGPUCorrelationTime
from scalartheorygpusimulation import ScalarTheoryGPUExpectationValueParameters
from scalartheorygpusimulation import ScalarTheoryGPUExpectationValue


from mcmc.mcmc_system import MCMCSystem


class ScalarTheoryGPU(MCMCSystem):
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSimParams = ScalarTheoryGPUEquilibriumTimeParameters
    EquiTimeSim = ScalarTheoryGPUEquilibriumTime
    CorrTimeSimParams = ScalarTheoryGPUCorrelationTimeParameters
    CorrTimeSim = ScalarTheoryGPUCorrelationTime
    ExpValSimParams = ScalarTheoryGPUExpectationValueParameters
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

    def initialize_model_parameters(self):
        # Required for mode simulation and mcmc simulation
        from scalartheorygpusimulation import ScalarTheoryGPUSystemParameters
        self._parameters = ScalarTheoryGPUSystemParameters(
            kappa=self.kappa, lambd=self.lambd, dimensions=self.dimensions,
            dt=self.dt, n=self.n, m=self.m
        )
        self._parameters.set_measures(measures=self.measure_names)

    def initialize(self, starting_mode):
        # Required for mcmc simulation
        from scalartheorygpusimulation import ScalarTheoryGPUSystem
        self.initialize_model_parameters()
        self._mcmc_system = ScalarTheoryGPUSystem(self._parameters)
        self._mcmc_system.init(starting_mode=starting_mode)