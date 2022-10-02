import sys
import os

import argparse

parser = argparse.ArgumentParser("Build Application")
parser.add_argument("-o", "--output-dir", help="Where to build the project.", type=str)
# parser.add_argument("--config-file", help="User configuration file", type=str, default=None)
parser.add_argument("-n", "--project_name", help="The name of the project.", type=str, default="my-mcmc-simulation-project")
parser.add_argument("-pdm", "--predefined_measures", help="Build project with a predinfed support of standard measures: 'Yes' or 'No' (default).", type=str, default="No")
parser.add_argument("-pyb", "--python_bindings", help="Whether building Python bindings should be support in the project: 'Yes' (default) or 'No'.", type=str, default="Yes")
parser.add_argument("-mt", "--main_template", help="Define the kind of template used as a starting point in the project: 'standard' (default), 'command_line_support' or 'cluster_support'.", type=str, default="standard")
# parser.add_argument("-cs", "--cuda_support", help="Whether the exeuction of CUDA code should be supported: 'Yes' or 'No' (default).", type=str, default="No")


args = parser.parse_args()

from cookiecutter.main import cookiecutter
cookiecutter(
    './application_wrapper',
    no_input=True,
    output_dir=args.output_dir,
    extra_context={
        'project_name': args.project_name,
        'use_predefined_measures': args.predefined_measures,
        'python_bindings': args.python_bindings,
        'main_template': args.main_template,
        'cuda_support': 'No' # args.cuda_support
    }
)

