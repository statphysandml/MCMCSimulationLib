
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

from mcmctools.mcmc.evaluation_module import EvaluationModule


class ModeSimulation(EvaluationModule):
    def __init__(self,
                 model,
                 sim_base_dir,
                 rel_data_dir=None,  # -> sim_base_dir + "/" + rel_data_dir
                 rel_results_dir=None,  # -> sim_base_dir + "/" + rel_results_dir
                 running_parameter_kind=None,
                 running_parameter=None,
                 rp_values=None,
                 python_modules_path=None, # Relative or absolute path which will be added by C++ to sys.path to correctly access custom measures ..custom load function..
                 **kwargs):
        super().__init__(sim_base_dir=sim_base_dir, rel_data_dir=rel_data_dir, rel_results_dir=rel_results_dir,
                         running_parameter_kind=running_parameter_kind, running_parameter=running_parameter,
                         rp_values=rp_values, **kwargs)

        if python_modules_path is None:
            self.python_modules_path = self.sim_base_dir
        else:
            self.python_modules_path = python_modules_path

        self.model = model
        self.model.initialize_mcmc_system()

    def _run_and_evaluate(self, SimClass, run, eval, simulation_mode_parameters):
        # Call constructor of Simulation Class
        from mcmcsimulation import ReadableMeasure
        measurement_system = ReadableMeasure(self.sim_base_dir + "/" + self.rel_data_dir)

        simulation = SimClass.generate_simulation(
            self.model._mcmc_system, simulation_mode_parameters, measurement_system,
            self.running_parameter_kind, self.running_parameter, self.rp_values
        )   
        if run:
            simulation.run()
        if eval:
            from mcmcsimulation import init_mcmc_python_binding
            init_mcmc_python_binding(self.python_modules_path)
            simulation.eval(self.sim_base_dir + "/" + self.rel_results_dir)

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
        from mcmcsimulation import EquilibriumTime
        equilibrium_time_parameters = EquilibriumTime(
            sample_size, number_of_steps, eval_confidence_range, eval_confidence_window, measure)
        self._run_and_evaluate(self.model._equilibrium_time_simulation_class(), run, eval, equilibrium_time_parameters)

    def correlation_time_simulation(self, run, eval,
                                    minimum_sample_size=100, maximum_correlation_time=1000, start_measuring=0,
                                    measure="Mean", starting_mode="hot",
                                    equilibrium_time_rel_results_dir="None"):
        from mcmcsimulation import CorrelationTime
        if equilibrium_time_rel_results_dir == "None":
            correlation_time_parameters = CorrelationTime(
                minimum_sample_size, maximum_correlation_time, start_measuring, measure, starting_mode)
        else:
            correlation_time_parameters = CorrelationTime(
                minimum_sample_size, maximum_correlation_time, self.sim_base_dir + "/" + equilibrium_time_rel_results_dir, measure, starting_mode)

        self._run_and_evaluate(self.model._correlation_time_simulation_class(), run, eval, correlation_time_parameters)

    def expectation_value_simulation(self, run, eval,
                                    measure_interval=100, number_of_measurements=1000, start_measuring=0,
                                    measures=["Mean"], eval_post_measures=[],
                                    starting_mode="hot", eval_error_type="statistical", eval_n_means_bootstrap=0,
                                    equilibrium_time_rel_results_dir="None",
                                    correlation_time_rel_results_dir="None"):

        from mcmcsimulation import ExpectationValue
        if equilibrium_time_rel_results_dir == "None" and correlation_time_rel_results_dir == "None":
            expectation_value_parameters = ExpectationValue(
                measure_interval, number_of_measurements, start_measuring, measures, eval_post_measures,
                starting_mode, eval_error_type, eval_n_means_bootstrap)
        elif equilibrium_time_rel_results_dir == "None":
            expectation_value_parameters = ExpectationValue(
                self.sim_base_dir + "/" + correlation_time_rel_results_dir, number_of_measurements, start_measuring, measures,
                eval_post_measures, starting_mode, eval_error_type, eval_n_means_bootstrap)
        elif correlation_time_rel_results_dir == "None":
            expectation_value_parameters = ExpectationValue(
                measure_interval, number_of_measurements, self.sim_base_dir + "/" + equilibrium_time_rel_results_dir, measures,
                eval_post_measures, starting_mode, eval_error_type, eval_n_means_bootstrap)
        else:
            expectation_value_parameters = ExpectationValue(
                self.sim_base_dir + "/" + correlation_time_rel_results_dir, number_of_measurements,
                self.sim_base_dir + "/" + equilibrium_time_rel_results_dir, measures,
                eval_post_measures, starting_mode, eval_error_type, eval_n_means_bootstrap)
        self._run_and_evaluate(self.model._expectation_value_simulation_class(), run, eval, expectation_value_parameters)
