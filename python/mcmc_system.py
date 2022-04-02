class MCMCSystem:
    def __init__(self, **kwargs):
        # Required for mcmc simulation
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
    
    def initialize_mcmc_system(self):
        # Needs to be implemented by the child class
        # Required for mode simulation and mcmc simulation
        assert False, "member function initalize_mcmc_system() needs to be implemented"

    def initialize(self, starting_mode):
        # Required for mcmc simulation
        self.initialize_mcmc_system()
        self._mcmc_system.init(starting_mode=starting_mode)

    @property
    def measure_names(self):
        return self._mcmc_system.measure_names()

    @measure_names.setter
    def measure_names(self, measures):
        self._mcmc_system.set_measures(measures=measures)
        
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