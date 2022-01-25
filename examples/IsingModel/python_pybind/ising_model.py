

class IsingModel:
    def __init__(self, beta, J, h, dimensions, **kwargs):
        super().__init__(**kwargs)

        # Required for mode simulation
        self.beta = beta
        self.J = J
        self.h = h
        self.dimensions = dimensions
        self.measures = []

        self.parameters = None

    def initialize(self):
        # Required for mode simulation and mcmc simulation
        from isingmodelsimulation import IsingModelParameters as SystemParameters
        self.parameters = SystemParameters.generate_parameters(
            beta=self.beta, J=self.J, h=self.h, dimensions=self.dimensions, measures=self.measures
        )

    def _equilibrium_time_simulation_classes(self):
        # Required for mode simulation
        from isingmodelsimulation import IsingModelMetropolisEquilibriumTimeParameters as EquiTimeSimParams
        from isingmodelsimulation import IsingModelMetropolisEquilibriumTime as EquiTimeSim
        return EquiTimeSimParams, EquiTimeSim

    def _correlation_time_simulation_classes(self):
        # Required for mode simulation
        from isingmodelsimulation import IsingModelMetropolisCorrelationTimeParameters as CorrTimeSimParams
        from isingmodelsimulation import IsingModelMetropolisCorrelationTime as CorrTimeSim
        return CorrTimeSimParams, CorrTimeSim

    def _expectation_value_simulation_classes(self):
        # Required for mode simulation
        from isingmodelsimulation import IsingModelMetropolisExpectationValueParameters as ExpValSimParams
        from isingmodelsimulation import IsingModelMetropolisExpectationValue as ExpValSim
        return ExpValSimParams, ExpValSim

    def set_measures(self, measures):
        # Required for mcmc simulation
        self.measures = measures

    def generate_simulation_model(self):
        # Required for  mcmc simulation
        from isingmodelsimulation import IsingModel as System
        return System(self.parameters)