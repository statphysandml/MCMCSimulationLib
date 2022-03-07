//
// Created by lukas on 31.07.19.
//

#ifndef PROGRAM_DEV_DAT_HPP
#define PROGRAM_DEV_DAT_HPP

#include "thrust_header.hpp"
#include "thrust_functors.hpp"

#include <utility>
#include <numeric>
#include <fstream>
#include <algorithm>

template <typename Iterator>
class DimensionIterator
{
public:
    DimensionIterator(const Iterator begin_iterator_, const Iterator end_iterator_) : begin_iterator(begin_iterator_), end_iterator(end_iterator_), N(end_iterator_ - begin_iterator_)
    {}

    const Iterator begin() const
    {
        return begin_iterator;
    }

    const Iterator end() const
    {
        return end_iterator;
    }

    uint size() const
    {
        return N;
    }

private:
    Iterator begin_iterator;
    Iterator end_iterator;
    uint N;
};

template<typename Vec, typename VecIterator, typename ConstVecIterator>
class DevDat : public Vec
{
public:
    DevDat()
    {
        dim = 0;
        N = 0;
        initialize_dimension_iterators();
    }

    DevDat(const uint dim_, const uint N_, const cudaT init_val = 0) : Vec(dim_ * N_, init_val), dim(dim_), N(N_)
    {
        initialize_dimension_iterators();
    }

    DevDat(const Vec device_data_, const uint dim_) : Vec(device_data_), dim(dim_), N(device_data_.size() / dim_)
    {
        initialize_dimension_iterators();
    }

    DevDat(std::vector< std::vector<double > > data) : DevDat(data[0].size(), data.size())
    {
        // Fill iterators with data
        for(int j = 0; j < dim; j++) {
            dev_iterator it = (*this)[j].begin();
            for (uint i = 0; i < N; i++) {
                *it = data[i][j];
                it++;
            }
        }
    }

    // Copy Constructor
    DevDat(const DevDat& b) : dim(b.dim), N(b.N), Vec(b)
    {
        initialize_dimension_iterators();
    }

    // Move constructor
    DevDat(DevDat&& other) noexcept : DevDat() // initialize via default constructor, C++11 only
    {
        thrust::swap(static_cast<Vec&>(*this), static_cast<Vec&>(other));
        swapp(*this, other);
    }

    // Move assignment
    DevDat & operator=(DevDat &&other ) // Changed on my own from no & to && (from DevDat other to &&other)
    {
        thrust::swap(static_cast<Vec&>(*this), static_cast<Vec&>(other));
        swapp(*this, other);
        return *this;
    }

    // Copy Assignement
    DevDat & operator=(const DevDat& other ) {
        return *this = DevDat(other);
    }

    const DimensionIterator<ConstVecIterator >& operator[] (int i) const
    {
        return const_dimension_iterators[i];
    }

    DimensionIterator<VecIterator >& operator[] (int i)
    {
        return dimension_iterators[i];
    }

    uint dim_size() const
    {
        return dim;
    }

    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    friend void swapp(DevDat& first, DevDat& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.dim, second.dim);
        swap(first.N, second.N);
        swap(first.dimension_iterators, second.dimension_iterators);
        swap(first.const_dimension_iterators, second.const_dimension_iterators);
    }

    template<typename Vecc>
    void fill_by_vec(Vecc &other)
    {
        Vec::operator=(other);
    }

    std::vector<double> get_ith_element(const int i) const
    {
        std::vector<double> ith_element(dim, 0);
        auto iterator = this->begin();
        // Jump to ith element in zeroth dimension
        thrust::advance(iterator, i);
        ith_element[0] = *iterator;
        // Fill further dimensions
        for(auto j = 1; j < dim; j++)
        {
            thrust::advance(iterator, N);
            ith_element[j] = *iterator;
        }
        return ith_element;
    }

    void set_dim(const uint dim_)
    {
        dim = dim_;
    }

    void set_N(const uint N_)
    {
        N = N_;
    }

    void initialize_dimension_iterators()
    {
        dimension_iterators.clear();
        const_dimension_iterators.clear();

        VecIterator begin = this->begin();
        VecIterator end = this->begin();
        thrust::advance(end, N);
        dimension_iterators.reserve(dim);
        for(auto i = 0; i < dim; i++)
        {
            dimension_iterators.push_back(DimensionIterator<VecIterator> (begin, end));
            const_dimension_iterators.push_back(DimensionIterator<ConstVecIterator> (begin, end));
            // std::cout << *begin << " " << *(end - 1) << std::endl;
            thrust::advance(begin, N);
            thrust::advance(end, N);
        }
    }

    // Converts the given device vector of size dim(=len) x n to a vector of vectors of size n(=len) x dim
    std::vector< std::vector<double > > transpose_device_data() const
    {
        // dim x total_number_of_coordinates (len = total_number_of_coordinates)
        // vs. total_number_of_coordinates x dim (len = dim)
        thrust::host_vector<cudaT> host_device_data(*this);

        std::vector< std::vector<double > > transposed_device_data(N, std::vector<double> (dim, 0));
        for(auto j = 0; j < dim; j++) {
            for (auto i = 0; i < N; i++) {
                transposed_device_data[i][j] = host_device_data[j * N + i];
            }
        }
        return transposed_device_data;
    }

private:
    uint dim;
    uint N;
    std::vector< DimensionIterator<VecIterator> > dimension_iterators;
    std::vector < DimensionIterator<ConstVecIterator> > const_dimension_iterators;
};


typedef DevDat<dev_vec, dev_iterator, const_dev_iterator> DevDatC;
typedef DevDat<dev_vec_int, dev_iterator_int, const_dev_iterator_int > DevDatInt;

typedef DimensionIterator< dev_iterator > DimensionIteratorC;
typedef DimensionIterator< const_dev_iterator > ConstDimensionIteratorC;

typedef DimensionIterator< dev_iterator_int > DimensionIteratorInt;
typedef DimensionIterator< const_dev_iterator_int > ConstDimensionIteratorInt;

typedef DevDat<dev_vec_bool, dev_iterator_bool, const_dev_iterator_bool > DevDatBool;

void write_data_to_ofstream(const DevDatC &data, std::ofstream &os, std::vector<int> skip_iterators_in_dimensions = std::vector<int>{}, std::vector< dev_iterator > end_iterators = std::vector< dev_iterator > {});

#endif //PROGRAM_DEV_DAT_HPP
