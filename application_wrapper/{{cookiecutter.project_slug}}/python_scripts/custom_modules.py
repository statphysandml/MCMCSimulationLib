

def compute_measures(data, measure_name, sim_params):
    from {{ cookiecutter.project_slug }}_measures import compute_{{ cookiecutter.project_slug }}_measures
    return compute_{{ cookiecutter.project_slug }}_measures(data=data, measure_name=measure_name, sim_params=sim_params)
