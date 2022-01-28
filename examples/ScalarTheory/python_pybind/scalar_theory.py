# ToDo:
# - Rewrite measurements as variant

from scalartheorysimulation import ScalarTheoryEquilibriumTimeParameters
from scalartheorysimulation import ScalarTheoryEquilibriumTime
from scalartheorysimulation import ScalarTheoryCorrelationTimeParameters
from scalartheorysimulation import ScalarTheoryCorrelationTime
from scalartheorysimulation import ScalarTheoryExpectationValueParameters
from scalartheorysimulation import ScalarTheoryExpectationValue


from mcmc.mcmc_system import MCMCSystem


class ScalarTheory(MCMCSystem): 
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSimParams = ScalarTheoryEquilibriumTimeParameters
    EquiTimeSim = ScalarTheoryEquilibriumTime
    CorrTimeSimParams = ScalarTheoryCorrelationTimeParameters
    CorrTimeSim = ScalarTheoryCorrelationTime
    ExpValSimParams = ScalarTheoryExpectationValueParameters
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

    def initialize_model_parameters(self):
        # Required for mode simulation and mcmc simulation
        from scalartheorysimulation import ScalarTheorySystemParameters
        self._parameters = ScalarTheorySystemParameters(
            kappa=self.kappa, lambd=self.lambd, dimensions=self.dimensions,
            dt=self.dt, n=self.n, m=self.m
        )
        self._parameters.set_measures(measures=self.measure_names)

    def initialize(self, starting_mode):
        # Required for mcmc simulation
        from scalartheorysimulation import ScalarTheorySystem
        self.initialize_model_parameters()
        self._mcmc_system = ScalarTheorySystem(self._parameters)
        self._mcmc_system.init(starting_mode=starting_mode)