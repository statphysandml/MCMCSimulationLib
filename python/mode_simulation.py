
# -*- coding: utf-8 -*-
"""Example Google style docstrings.

This module demonstrates documentation as specified by the `Google Python
Style Guide`_. Docstrings may extend over multiple lines. Sections are created
with a section header and a colon followed by a block of indented text.

Example:
    Examples can be given using either the ``Example`` or ``Examples``
    sections. Sections support any reStructuredText formatting, including
    literal blocks::

        $ python example_google.py

Section breaks are created by resuming unindented text. Section breaks
are also implicitly created anytime a new section starts.

Attributes:
    module_level_variable1 (int): Module level variables may be documented in
        either the ``Attributes`` section of the module docstring, or in an
        inline docstring immediately following the variable.

        Either form is acceptable, but the two should not be mixed. Choose
        one convention to document module level variables and be consistent
        with it.

Todo:
    * For module TODOs
    * You have to also use ``sphinx.ext.todo`` extension

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from mcmctools.evaluation_module import EvaluationModule


# ToDo: Implement simulation with non_equal rp_keys
class ModeSimulation(EvaluationModule):
    def __init__(self,
                 model,
                 sim_base_dir=None,
                 rel_data_path=None,  # -> sim_base_dir + "/" + rel_data_path
                 rel_results_path=None,  # -> sim_base_dir + "/" + rel_results_path
                 running_parameter_kind=None,
                 running_parameter=None,
                 rp_keys=None,
                 rp_intervals=None, **kwargs):
        print("Calling Simulation Mode Module Base Head")
        super().__init__(sim_base_dir=sim_base_dir, rel_data_path=rel_data_path, rel_results_path=rel_results_path,
                         running_parameter_kind=running_parameter_kind, running_parameter=running_parameter,
                         rp_keys=rp_keys, rp_intervals=rp_intervals, **kwargs)
        print("Calling Simulation Mode Module Base Body")

        self.model = model
        self.model.initialize_parameters()

    def _run_and_evaluate(self, SimParamClass, SimClass, run, eval, simulation_mode_parameters):
        simulation_parameters = SimParamClass.generate_simulation(
            self.model.parameters, simulation_mode_parameters,
            self.rel_data_path,
            self.running_parameter_kind, self.running_parameter, self.rp_intervals
        )
        simulation = SimClass(simulation_parameters)
        if run:
            simulation.run()
        if eval:
            from mcmcsimulation import init_mcmc_python_binding
            init_mcmc_python_binding("./") # ToDo  Adapt Path!
            simulation.evaluate(self.rel_results_path, "./")
        # assert False, "This should be also possible from file!" <-> i.e. wihtout the need to provide any parameters for the class
        # -> possilbe solutions solution:
        #  - write second constructor for loading lattice parameters form file? important distinugish between config files and json files in data dir
        #  - or provide to this without the need to important any necessary class by generating a c++ file containing all possibl combinations (suboptimal!)

    def equilibrium_time_simulation(self, run: bool, eval: bool,
                                         sample_size: int=100, number_of_steps: int=1000, eval_confidence_range: float=0.1,
                                         eval_confidence_window: int=10, measure: str="Mean") -> None:
        """Example function with types documented in the docstring.

        `PEP 484`_ type annotations are supported. If attribute, parameter, and
        return types are annotated according to `PEP 484`_, they do not need to be
        included in the docstring:

        :param run: Indicate wheter ....
        :param eval: The second parameter.
        :param sample_size: ...asd

        Returns:
            None: The return value. True for success, False otherwise.

        .. _PEP 484:
            https://www.python.org/dev/peps/pep-0484/
        

        """
        from mcmcsimulation import EquilibriumTimeParameters
        equilibrium_time_parameters = EquilibriumTimeParameters(
            sample_size, number_of_steps, eval_confidence_range, eval_confidence_window, measure)
        self._run_and_evaluate(*self.model._equilibrium_time_simulation_classes(), run, eval, equilibrium_time_parameters)

    def correlation_time_simulation(self, run, eval,
                                         minimum_sample_size=100, maximum_correlation_time=1000, start_measuring=0,
                                         measure="Mean", starting_mode="hot",
                                         equilibrium_time_rel_results_path="None"):
        from mcmcsimulation import CorrelationTimeParameters
        if equilibrium_time_rel_results_path is "None":
            correlation_time_parameters = CorrelationTimeParameters(
                minimum_sample_size, maximum_correlation_time, start_measuring, measure, starting_mode)
        else:
            correlation_time_parameters = CorrelationTimeParameters(
                minimum_sample_size, maximum_correlation_time, equilibrium_time_rel_results_path, measure, starting_mode)

        self._run_and_evaluate(*self.model._correlation_time_simulation_classes(), run, eval, correlation_time_parameters)

    def expectation_value_simulation(self, run, eval,
                                          measure_interval=100, number_of_measurements=1000, start_measuring=0,
                                          measures=["Mean"], eval_post_measures=[],
                                          eval_n_means_bootstrap=0, starting_mode="hot", equilibrium_time_rel_results_path="None",
                                          correlation_time_rel_results_path="None"):

        from mcmcsimulation import ExpectationValueParameters
        if equilibrium_time_rel_results_path is "None" and correlation_time_rel_results_path is "None":
            expectation_value_parameters = ExpectationValueParameters(
                measure_interval, number_of_measurements, start_measuring, measures, eval_post_measures,
                eval_n_means_bootstrap, starting_mode)
        elif equilibrium_time_rel_results_path is "None":
            expectation_value_parameters = ExpectationValueParameters(
                correlation_time_rel_results_path, number_of_measurements, start_measuring, measures,
                eval_post_measures, eval_n_means_bootstrap, starting_mode)
        elif correlation_time_rel_results_path is "None":
            expectation_value_parameters = ExpectationValueParameters(
                measure_interval, number_of_measurements, equilibrium_time_rel_results_path, measures,
                eval_post_measures, eval_n_means_bootstrap, starting_mode)
        else:
            expectation_value_parameters = ExpectationValueParameters(
                correlation_time_rel_results_path, number_of_measurements, equilibrium_time_rel_results_path, measures,
                eval_post_measures, eval_n_means_bootstrap, starting_mode)
        self._run_and_evaluate(*self.model._expectation_value_simulation_classes(), run, eval, expectation_value_parameters)
