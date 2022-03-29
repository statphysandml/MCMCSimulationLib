class MCMCSystem:
    def __init__(self, **kwargs):
        # Required for mcmc simulation
        self.measure_names = [] # Default used for mode simulations <-> measures will be set automatically by the mode simulators
        self._mcmc_system = None

    @classmethod
    def _equilibrium_time_simulation_class(cls):
        # Required for mode simulation
        return cls.EquiTimeSim

    @classmethod
    def _correlation_time_simulation_class(cls):
        # Required for mode simulation
        return cls.CorrTimeSim

    @classmethod
    def _expectation_value_simulation_class(cls):
        # Required for mode simulation
        return cls.ExpValSim

    @property
    def model_parameters(self):
        # Required for mode simulation
        return self._parameters

    @property
    def measure_names(self):
        return self._measures

    @measure_names.setter
    def measure_names(self, measures):
        self._measures = measures
        
    def update(self, n_step):
        self._mcmc_system.update(n_step)

    def measure(self):
        return self._mcmc_system.measure()
    
    def size(self):
        return self._mcmc_system.size()
    
    def __getitem__(self, i):
        return self._mcmc_system[i]
    
    def __setitem__(self, i, val):
        self._mcmc_system[i] = val
    
    @property
    def system_repr(self):
        return self._mcmc_system.system_repr

    @system_repr.setter
    def system_repr(self, system):
        self._mcmc_system.system_repr = system