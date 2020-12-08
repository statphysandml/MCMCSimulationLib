ParamHelper: Track your simulation
==================================

ParamHelper is a C++ library that allows an easy tracking of all parameters of a simulation. The parameters can easily be written to file and be loaded for a potential rerun of the simulation. An example provides a possible usage of the library. The core of the library makes use of JSON for Modern C++ from https://github.com/nlohmann/json. The parameters a managed based on json object.

Use Case
--------

Possible usages of the library can be best understood through examples. More detailed examples can also be found in the examples/ directory.

Use case: We want to manange the parameters of a class Rectangle that can be used to compute the are of rectangle

The class RectangleParameter contains all parameters that are supossed to be tracked:
```c++
#include "param_helper/params.hpp"
using namespace param_helper::params;

class RectangleParameters : public Parameters
{
public:
    explicit RectangleParameters(const json params) : Parameters(params),
                                                      length(get_entry<double>("length", 1.0)),
                                                      width(get_entry<double>("width", 1.0))
    {}

    explicit RectangleParameters(const double length_=1.0, const double width_=1.0) :
        RectangleParameters(json {{"length", length_}, {"width", width_}})
    {}

    static std::string name()
    {
        return "Rectangle";
    }

private:
    friend class Rectangle;

    double length;
    double width;
};
```

The actual class Rectangle has been defined as a friend to the class RectangleParameter. The class implements all functionalities:
```c++
class Rectangle
{
public:

    explicit Rectangle(const RectangleParameters rp_) : rp(rp_)
    {}

    double get_area() const
    {
        return rp.length * rp.width;
    }

private:
    const RectangleParameters rp;
};
```
Access on the parameters is provided by the member variable rp. The class Reactangle can be used as follows:
```c++
// The project root can be adapted by params_helper::prfs::set_relative_path_to_project_root_dir
std::cout << "Current directory: " << param_helper::fs::prfs::project_root() << std::endl;

// Generate parameters and write them to file
RectangleParameters rp(2.0, 3.0);
// Generates the directory parameters/ relative to the project_root
param_helper::fs::generate_directory_if_not_present("parameters", true);
rp.write_to_file("parameters", "rectangle_parameters", true);

// Optional
param_helper::fs::check_if_parameter_file_exists("parameters", "rectangular_parameters");

// Reload the parameters
auto params2 = Parameters::create_by_file("parameters", "rectangle_parameters");
RectangleParameters rp2(params2.get_json());

// Alternative way
RectangleParameters rp3(
    param_helper::fs::read_parameter_file("parameters", "rectangle_parameters", true)
);

// Generate object with respective parameters
Rectangle rectangle2(rp3);

std::cout << "\nCompute rectangle area: " << rectangle2.get_area() << "\n" << std::endl;
```

In the example, a json file "rectangle_parameters.json" has been stored in the directory "parameters":
```json
{
    "length": 2.0,
    "width": 3.0
}

```

Build
-----

The library can be easily build by
```bash
cd build
bash build.sh
```

Usage
-----

g++:
```bash
g++ main.cpp -I {path-to-ParamHelper/include/} {path-to-ParamHelper/lib/libparamhelper.a} -o main
```

cmake (CMakeLists.txt):
```cmake
find_library(ParamHelper NAMES libparamhelper.a PATHS {path-to-ParamHelper/lib/`})
include_directories({path-to-ParamHelper/include/})
target_link_libraries(MAIN ${ParamHelper})
```

Further Examples
----------------

### Use case with multiple parameters

The library also provides an easy way to combine several parameter files
```c++
Parameters project_params = Parameters::create_by_params(
    json {{"name", "project_a"}, {"details", "rectangle_analysis"}}
);

RectangleParameters rp_analysis(2.0, 3.0);
project_params.append_parameters(rp_analysis);

param_helper::fs::generate_directory_if_not_present("project", true);
project_params.write_to_file("project", "rectangle_analysis", true);

// Add additional_project parameters to the already existing project file based on the given path
Parameters additional_project_params = Parameters::create_by_params(
    json {{"type", "type_b"}, {"n", "100"}}
);
additional_project_params.merge_to_file("project", "rectangle_analysis", true);

// Reload updated parameters
auto updated_project_params = Parameters::create_by_file("project", "rectangle_analysis");
std::cout << "Updated project params " << updated_project_params << std::endl;
```
A folder "project" has been created that contains a "rectangle_analysis.json" file, that contains also the rectangle parameters:
```json
{
    "Rectangle": {
        "length": 2.0,
        "width": 3.0
    },
    "details": "rectangle_analysis",
    "n": "100",
    "name": "project_a",
    "type": "type_b"
}
```

### The Parameter class

Examples for using the base class Parameter. The class Parameter is a wrapper to manage access and further functions on a nohlmann::json object.


```c++
#include "param_helper/params.hpp"
using namespace param_helper::params;

Parameters params = Parameters::create_by_params(
    json {{"a", 0}, {"vec", std::vector<double> {0.0, 1.0}}}
);
params.add_entry("c", "c");

std::cout << "Parameters: " << params << std::endl;

auto a = params.get_entry<double>("a");
auto b = params.get_entry<double>("b", 1.0, true);
std::cout << "Extracted parameters: a " << a << " " << "b " << b << "\n" << std::endl;

std::cout << "Looking for parameter b: " << params.haskey("b") << std::endl;

// Delete entry with "b"
params.delete_entry("b");

// Get raw nohlmann json file
json params_json = params.get_json();
std::cout << "\nParams json object: " << params_json << "\n" << std::endl;
```

### Manipulating json objects

Manipulation of json objects (merge and subtract)

```c++
json additional_parameters = {{"e", 0.0}, {"f", std::complex<double> {1.0, 1.0}}};
params_json = merge(params_json, additional_parameters);
params_json = subtract(params_json, additional_parameters);
```