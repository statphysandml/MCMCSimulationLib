# {{ cookiecutter.project_name }}

The generated project serves as a template project for implementing your own MCMC simulation.
{# The white-space control of the below template is quite delicate - if you add one, do it exactly like this (mind the -'s) -#}
{%- set is_github = "github.com" in cookiecutter.remote_url -%}
{%- set is_gitlab = "gitlab" in cookiecutter.remote_url -%}
{%- set provider_acronym = "gh" if is_github else "gl" if is_gitlab else "none" %}
{%- set username = "None" if cookiecutter.remote_url == "None" else cookiecutter.remote_url.split("/")[-2].split(":")[-1] -%}
{%- set remote_slug = "None" if cookiecutter.remote_url == "None" else cookiecutter.remote_url.replace(".git", "").split("/")[-1] -%}
{%- set gitlab_instance = "None" if cookiecutter.remote_url == "None" else "https%3A%2F%2F" + cookiecutter.remote_url.replace("https://", "").replace("ssh://git@", "").split("/")[0].split(":")[0] -%}
{%- set python_package = cookiecutter.project_slug.replace("-", "") -%}
{% if cookiecutter.license == "MIT" -%}
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
{% endif -%}
{% if cookiecutter.license == "BSD-2" -%}
[![License](https://img.shields.io/badge/License-BSD%202--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)
{% endif -%}
{% if cookiecutter.license == "GPL-3.0" -%}
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
{% endif -%}
{% if cookiecutter.license == "LGPL-3.0" -%}
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
{% endif -%}
{% if cookiecutter.remote_url != "None" -%}
{% if cookiecutter.github_actions_ci == "Yes" and is_github -%}
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/{{ username }}/{{ remote_slug }}/CI)](https://github.com/{{ username }}/{{ remote_slug }}/actions?query=workflow%3ACI)
{% endif -%}
{% if cookiecutter.gitlab_ci == "Yes" and is_gitlab -%}
[![Gitlab pipeline status](https://img.shields.io/gitlab/pipeline/{{ username }}/{{ remote_slug }}/main
{%- if "gitlab.com" not in cookiecutter.remote_url -%}
?gitlab_url={{ gitlab_instance }}
{%- endif -%}
)]({{ gitlab_instance }}/{{ username }}/{{ remote_slug }}/-/pipelines)
{% endif -%}
{% endif -%}
{% if cookiecutter.pypi_release != "No" -%}
[![PyPI Release](https://img.shields.io/pypi/v/{{ python_package }}.svg)](https://pypi.org/project/{{ python_package }})
{% endif -%}
{% if cookiecutter.readthedocs == "Yes" -%}
[![Documentation Status](https://readthedocs.org/projects/{{ remote_slug }}/badge/)](https://{{ remote_slug }}.readthedocs.io/)
{% endif -%}
{% if cookiecutter.codecovio == "Yes" -%}
[![codecov](https://codecov.io/{{ provider_acronym }}/{{ username }}/{{ remote_slug }}/branch/main/graph/badge.svg)](https://codecov.io/{{ provider_acronym }}/{{ username }}/{{ remote_slug }})
{%- endif %}
{% if cookiecutter.sonarcloud == "Yes" -%}
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project={{ username }}_{{ remote_slug }}&metric=alert_status)](https://sonarcloud.io/dashboard?id={{ username }}_{{ remote_slug }})
{%- endif %}
{{ "\n" -}}
# Prerequisites

Building {{ cookiecutter.project_name }} requires the following software installed:

* A C++{{ cookiecutter.cxx_minimum_standard }}-compliant compiler
* CMake `>= 3.15`
* MCMCSimulationLib
{%- if cookiecutter.doxygen == "Yes" or cookiecutter.readthedocs == "Yes" %}
* Doxygen (optional, documentation building is skipped if missing)
{%- endif %}
{%- if cookiecutter.use_submodules == "No" and cookiecutter.build_tests == "Yes" %}
* The testing framework [Catch2](https://github.com/catchorg/Catch2) for building the test suite
{%- endif %}
{%- if cookiecutter.python_bindings == "Yes" -%}
* Python `>= 3.6` for building Python bindings
{%- endif %}
{%- if cookiecutter.use_submodules == "No" and cookiecutter.python_bindings == "Yes" -%}
* The [PyBind11](https://github.com/pybind/pybind11) library for building Python bindings
{%- endif %}

# Building {{ cookiecutter.project_name }}

The following sequence of commands builds {{ cookiecutter.project_name }}. If you use a virtual envirnonment, it is important that it is activated for the building process to find the correct python version. The sequence assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The build process can be again customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF} / {other}` to the `cmake` call:

* `CLUSTER_MODE`: Indicates whether the code is executed on the cluster (`on_cluster`) or Device::locally (`local`) (default: `local`). The "local" option can be used to test whether the library prepares a computation on a cluster correctly. In this case, the simulation runs Device::locally on your machine. The option can be changed to "on_cluster". In this case the jobs are sent to the cluster. There are two functions "prepare_execution_on_cpu_cluster" and "run_execution_on_cpu_cluster" that take care of this. The functions can be found in the file src/execution/execution.cpp and need to be adapted according to the used cluster. More details on how to execute a simulation on the cluster can be found in the main.cpp file of the SimulateAndExecute example (https://github.com/statphysandml/MCMCSimulationLib/blob/master/examples/SimulateAndExecute//src/main.cpp) or in the main.cpp file of a template project (see Template Project).
* `PYTHON_SCRIPTS_PATH`: Path to a directory including additional python files for a possible execution of code of custom written functions and modules. (default: `./python_scripts`). The path is appended by the programming to sys.path. It needs to be defined relative to the project root path.