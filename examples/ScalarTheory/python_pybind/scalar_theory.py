

class ScalarTheory:
    def __init__(self, kappa, lambd, dimensions, dt, n, m, **kwargs):
        super().__init__(**kwargs)

        # Required for mode simulation
        self.kappa = kappa
        self.lambd = lambd
        self.dimensions = dimensions
        self.dt = dt
        self.n = n
        self.m = m

        self.measures = []

        self.parameters = None

    def initialize(self):
        # Required for mode simulation and mcmc simulation
        from scalartheorysimulation import ScalarTheoryParameters as SystemParameters
        self.parameters = SystemParameters.generate_parameters(
            kappa=self.kappa, lambd=self.lambd, dimensions=self.dimensions,
            dt=self.dt, n=self.n, m=self.m, measures=self.measures
        )

    def _equilibrium_time_simulation_classes(self):
        # Required for mode simulation
        from scalartheorysimulation import ScalarTheoryEquilibriumTimeParameters as EquiTimeSimParams
        from scalartheorysimulation import ScalarTheoryEquilibriumTime as EquiTimeSim
        return EquiTimeSimParams, EquiTimeSim

    def _correlation_time_simulation_classes(self):
        # Required for mode simulation
        from scalartheorysimulation import ScalarTheoryCorrelationTimeParameters as CorrTimeSimParams
        from scalartheorysimulation import ScalarTheoryCorrelationTime as CorrTimeSim
        return CorrTimeSimParams, CorrTimeSim

    def _expectation_value_simulation_classes(self):
        # Required for mode simulation
        from scalartheorysimulation import ScalarTheoryExpectationValueParameters as ExpValSimParams
        from scalartheorysimulation import ScalarTheoryExpectationValue as ExpValSim
        return ExpValSimParams, ExpValSim

    def set_measures(self, measures):
        # Required for mcmc simulation
        self.measures = measures

    def generate_simulation_model(self):
        # Required for  mcmc simulation
        from scalartheorysimulation import ScalarTheory as System
        return System(self.parameters)