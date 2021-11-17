import sys
import os

import argparse

parser = argparse.ArgumentParser("Build Application")
parser.add_argument("-o", "--output-dir", help="Where to output the generated project dir into.", type=str)
parser.add_argument("--config-file", help="User configuration file", type=str, default=None)
parser.add_argument("-n", "--project_name", help="The name of the project.", type=str, default="my-mcmc-simulation-project")
args = parser.parse_args()

from cookiecutter.main import cookiecutter
cookiecutter(
    './application_wrapper',
    no_input=True,
    output_dir=args.output_dir,
    config_file=args.config_file,
    extra_context={
        'path_to_mcmc_simulation_lib': os.path.dirname(os.path.realpath(__file__)),
        'project_name': args.project_name
    }
)

