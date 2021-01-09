cluster_mode="local"
# (optional - default=local) local/on_cluster - Can be adapted temporarily by
# adding -DCLUSTER_MODE=".." to the cmake command
# - "local" = for testing - does not actually start the code on the cluster but locally
# and performs all the necessary preparation
# - "on_cluster" = for the actual execution on a cluster
python_modules_path="~/MCMCSimulationLib/examples/python_scripts"
# (optional - default="./python_scripts" for projects and "./../python_scripts/" for
# simulations.) for a possible execution code of custom written functions and modules.
# The directory "python_modules_path" is added to sys.path by the program.
# The path needs to be defined relative to the project root path)