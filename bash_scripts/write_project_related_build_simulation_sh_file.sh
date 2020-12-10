cat >$project_path/bash_scripts/build_simulation.sh <<EOL
#!/bin/bash

path_to_mcmc_simulation_lib="${path_to_mcmc_simulation_lib}"
source "\${path_to_mcmc_simulation_lib}/bash_scripts/build_simulation.sh"

EOL