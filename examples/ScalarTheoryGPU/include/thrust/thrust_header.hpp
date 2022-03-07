//
// Created by lukas on 17.09.20.
//

#ifndef LATTICEMODELIMPLEMENTATIONS_THRUST_HEADER_HPP
#define LATTICEMODELIMPLEMENTATIONS_THRUST_HEADER_HPP

#include <iostream>

#if defined(GPU)

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

using namespace thrust::placeholders;

typedef double cudaT;

typedef thrust::device_vector<cudaT> dev_vec;
typedef thrust::device_vector<int> dev_vec_int;
typedef thrust::device_vector<bool> dev_vec_bool;
typedef thrust::host_vector< dev_vec_int * > dev_ptrvec_vec_int;

typedef thrust::device_vector<cudaT>::iterator dev_iterator;
typedef thrust::device_vector<int>::iterator dev_iterator_int;
typedef thrust::device_vector<bool>::iterator dev_iterator_bool;

typedef thrust::device_vector<cudaT>::const_iterator const_dev_iterator;
typedef thrust::device_vector<int>::const_iterator const_dev_iterator_int;
typedef thrust::device_vector<bool>::const_iterator const_dev_iterator_bool;

typedef thrust::device_vector< dev_iterator_int* > dev_iter_vec_int;

typedef thrust::device_vector<cudaT*> dev_vec_ptrcudaT;
typedef thrust::host_vector<dev_vec_ptrcudaT> dev_vec_vec_ptrcudaT;

typedef thrust::host_vector< dev_vec_int> dev_vec_vec_int;

#else

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

using namespace thrust::placeholders;

typedef double cudaT;

typedef thrust::host_vector<cudaT> dev_vec;
typedef thrust::host_vector<int> dev_vec_int;
typedef thrust::host_vector<bool> dev_vec_bool;
typedef thrust::host_vector< dev_vec_int * > dev_ptrvec_vec_int;

typedef thrust::host_vector<cudaT>::iterator dev_iterator;
typedef thrust::host_vector<int>::iterator dev_iterator_int;
typedef thrust::host_vector<bool>::iterator dev_iterator_bool;

typedef thrust::host_vector<cudaT>::const_iterator const_dev_iterator;
typedef thrust::host_vector<int>::const_iterator const_dev_iterator_int;
typedef thrust::host_vector<bool>::const_iterator const_dev_iterator_bool;

typedef thrust::host_vector< dev_iterator_int* > dev_iter_vec_int;

typedef thrust::host_vector<cudaT*> dev_vec_ptrcudaT;
typedef thrust::host_vector<dev_vec_ptrcudaT> dev_vec_vec_ptrcudaT;

typedef thrust::host_vector< dev_vec_int> dev_vec_vec_int;

#endif


void print_system_info();

#endif //LATTICEMODELIMPLEMENTATIONS_THRUST_HEADER_HPP
