#include "../../include/thrust/dev_dat.hpp"

// Converts a device vector of size dim(=len) x n to a vector of vectors of size n(=len) x dim
/* std::vector< std::vector<double > > transpose_device_data(DevDatC device_data)
{
    std::vector< std::vector<double > > transposed_device_data(device_data[0].size(), std::vector<double> (device_data.dim_size(), 0));
    for(auto j = 0; j < device_data.dim_size(); j++) {
        thrust::host_vector<cudaT> device_data_dim_j(device_data[j].size());
        thrust::copy(device_data[j].begin(), device_data[j].end(), device_data_dim_j.begin());
        for (auto i = 0; i < device_data_dim_j.size(); i++) {
            transposed_device_data[i][j] = device_data_dim_j[i];
        }
    }
    return transposed_device_data;
} */

// Reverts tranpose_device_data
/* DevDatC transpose_to_device_data(std::vector< std::vector<double > > data)
{
    DevDatC transposed_data(data[0].size(), data.size());
    for(auto j = 0; j < data[0].size(); j++) {
        dev_iterator it = transposed_data[j].begin();
        for (auto i = 0; i < data.size(); i++) {
            *it = data[i][j];
            it++;
        } */
        /* thrust::host_vector<cudaT> device_data_dim_j(data.size());
        for (auto i = 0; i < data.size(); i++) {
            device_data_dim_j[i] = data[i][j];
        }
        transposed_data[j] = new dev_vec(device_data_dim_j); */
    /* }
    return transposed_data;
} */


void write_data_to_ofstream(const DevDatC &data, std::ofstream &os, std::vector<int> skip_iterators_in_dimensions, std::vector< dev_iterator > end_iterators)
{
    std::vector< const_dev_iterator > data_iterators;
    for(auto i = 0; i < data.dim_size(); i++)
    {
        if(std::find(skip_iterators_in_dimensions.begin(), skip_iterators_in_dimensions.end(), i) == skip_iterators_in_dimensions.end())
            data_iterators.push_back(data[i].begin());
    }

    auto data_num = data[0].size();
    if(end_iterators.size() > 0)
        data_num = end_iterators[0] - data[0].begin();

    // Data is written transposed to file by incrementing the data iterators in each step
    for(unsigned long int i = 0; i < data_num; i++)
    {
        for(auto &data_iter: data_iterators)
        {
            os << *data_iter << " ";
            data_iter++;
        }
        os << std::endl;
    }
}