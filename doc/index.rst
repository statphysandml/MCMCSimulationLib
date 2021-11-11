{{ cookiecutter._project_name}}
{% for c in "{{ cookiecutter._project_name}}" %}={% endfor %}

.. toctree::
   :maxdepth: 2
   :caption: Contents:

This is an example function:

.. _doxygenfunction:: {{ cookiecutter._project_slug.replace("-", "") }}::add_one
