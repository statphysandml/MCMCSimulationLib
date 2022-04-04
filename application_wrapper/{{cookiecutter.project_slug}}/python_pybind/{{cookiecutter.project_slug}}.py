from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}EquilibriumTime
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}CorrelationTime
from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}ExpectationValue


from mcmc.mcmc_system import MCMCSystem


class {{ cookiecutter.project_name }}(MCMCSystem):
    # Static variables used by parent classmethods for mode simulation
    EquiTimeSim = {{ cookiecutter.project_name }}EquilibriumTime
    CorrTimeSim = {{ cookiecutter.project_name }}CorrelationTime
    ExpValSim = {{ cookiecutter.project_name }}ExpectationValue

    def __init__(self, mu, sigma, dt, **kwargs):
        super().__init__(**kwargs)
        # Required for mode simulation and mcmc simulation
        self.mu = mu
        self.sigma = sigma
        self.dt = dt

    def initialize_mcmc_system(self):
        # Required for mode simulation and mcmc simulation
        from {{ cookiecutter.project_slug }}simulation import {{ cookiecutter.project_name }}System
        self._mcmc_system = {{ cookiecutter.project_name }}System(
            mu=self.mu, sigma=self.sigma, dt=self.dt
        )
