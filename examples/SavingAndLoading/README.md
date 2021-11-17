# Welcome to examples




# Prerequisites

Building examples requires the following software installed:

* A C++14-compliant compiler
* CMake `>= 3.9`

# Building examples

The following sequence of commands builds examples.
It assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF}` to the `cmake` call:

* RUN_WITH_PYTHON_BACKEND
* CLUSTER_MODE
* PYTHON_SCRIPTS_PATH

# Documentation

examples *should* provide a documentation.
