# cluster_mode="local"
# (optional - default=local) local/on_cluster - Can be adapted temporarily by adding -DCLUSTER_MODE=".." to the cmake command
# - local = for testing - does not actually start the code on the cluster but performs all the preparation
# - on_cluster = for execution on cluster - run code on cluster
# python_modules_path="~/MCMCSimulationLib/examples/python_scripts" # (for possible execution code of custom written functions and modules - is added to sys.path by the program)
# (optional - default="./../python_scripts" for projects and "./../../python_scripts/" for simulations.