from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}EquilibriumTimeParameters
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}EquilibriumTime
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}CorrelationTimeParameters
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}CorrelationTime
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}ExpectationValueParameters
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}ExpectationValue


from mcmc.mcmc_system import MCMCSystem


class {{ cookiecutter.project_name }}(MCMCSystem):
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSimParams = {{ cookiecutter.project_name }}EquilibriumTimeParameters
    EquiTimeSim = {{ cookiecutter.project_name }}EquilibriumTime
    CorrTimeSimParams = {{ cookiecutter.project_name }}CorrelationTimeParameters
    CorrTimeSim = {{ cookiecutter.project_name }}CorrelationTime
    ExpValSimParams = {{ cookiecutter.project_name }}ExpectationValueParameters
    ExpValSim = {{ cookiecutter.project_name }}ExpectationValue

    def __init__(self, mu, sigma, dt, **kwargs):
        super().__init__(**kwargs)
        # Required for mode simulation and mcmc simulation
        self.mu = mu
        self.sigma = sigma
        self.dt = dt

    def initialize_model_parameters(self):
        # Required for mode simulation and mcmc simulation
        from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}SystemParameters
        self._parameters = {{ cookiecutter.project_name }}SystemParameters(
            mu=self.mu, sigma=self.sigma, dt=self.dt
        )
        self._parameters.set_measures(measures=self.measure_names)

    def initialize(self, starting_mode):
        # Required for mcmc simulation
        from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}System
        self.initialize_model_parameters()
        self._mcmc_system = {{ cookiecutter.project_name }}System(self._parameters)
        self._mcmc_system.init(starting_mode=starting_mode)
