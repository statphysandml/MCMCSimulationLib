#include "../../include/thrust/thrust_header.hpp"

void print_system_info()
{
    int driver_version , runtime_version;
    cudaDriverGetVersion( &driver_version );
    cudaRuntimeGetVersion ( &runtime_version );
    std::cout << driver_version << "\t" << runtime_version << std::endl;
}