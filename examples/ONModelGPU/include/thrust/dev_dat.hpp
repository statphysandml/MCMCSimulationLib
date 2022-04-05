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
    DimensionIterator(const Iterator begin_iterator, const Iterator end_iterator) : begin_iterator_(begin_iterator), end_iterator_(end_iterator), N_(end_iterator - begin_iterator)
    {}

    const Iterator begin() const
    {
        return begin_iterator_;
    }

    const Iterator end() const
    {
        return end_iterator_;
    }

    uint size() const
    {
        return N_;
    }

private:
    Iterator begin_iterator_;
    Iterator end_iterator_;
    uint N_;
};

template<typename Vec, typename VecIterator, typename ConstVecIterator>
class DevDat : public Vec
{
public:
    DevDat()
    {
        dim_ = 0;
        N_ = 0;
        initialize_dimension_iterators();
    }

    DevDat(const uint dim, const uint N, const cudaT init_val = 0) : Vec(dim * N, init_val), dim_(dim), N_(N)
    {
        initialize_dimension_iterators();
    }

    DevDat(const Vec device_data, const uint dim) : Vec(device_data), dim_(dim), N_(device_data.size() / dim)
    {
        initialize_dimension_iterators();
    }

    DevDat(std::vector< std::vector<double > > data) : DevDat(data[0].size(), data.size())
    {
        // Fill iterators with data
        for(int j = 0; j < dim_; j++) {
            dev_iterator it = (*this)[j].begin();
            for (uint i = 0; i < N_; i++) {
                *it = data[i][j];
                it++;
            }
        }
    }

    // Copy Constructor
    DevDat(const DevDat& b) : dim_(b.dim_), N_(b.N_), Vec(b)
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
        return const_dimension_iterators_[i];
    }

    DimensionIterator<VecIterator >& operator[] (int i)
    {
        return dimension_iterators_[i];
    }

    uint dim_size() const
    {
        return dim_;
    }

    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    friend void swapp(DevDat& first, DevDat& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.dim_, second.dim_);
        swap(first.N_, second.N_);
        swap(first.dimension_iterators_, second.dimension_iterators_);
        swap(first.const_dimension_iterators_, second.const_dimension_iterators_);
    }

    template<typename Vecc>
    void fill_by_vec(Vecc &other)
    {
        Vec::operator=(other);
    }

    std::vector<double> get_ith_element(const int i) const
    {
        std::vector<double> ith_element(dim_, 0);
        auto iterator = this->begin();
        // Jump to ith element in zeroth dimension
        thrust::advance(iterator, i);
        ith_element[0] = *iterator;
        // Fill further dimensions
        for(auto j = 1; j < dim_; j++)
        {
            thrust::advance(iterator, N_);
            ith_element[j] = *iterator;
        }
        return ith_element;
    }

    void set_dim(const uint dim)
    {
        dim_ = dim;
    }

    void set_N(const uint N)
    {
        N_ = N;
    }

    void initialize_dimension_iterators()
    {
        dimension_iterators_.clear();
        const_dimension_iterators_.clear();

        VecIterator begin = this->begin();
        VecIterator end = this->begin();
        thrust::advance(end, N_);
        dimension_iterators_.reserve(dim_);
        for(auto i = 0; i < dim_; i++)
        {
            dimension_iterators_.push_back(DimensionIterator<VecIterator> (begin, end));
            const_dimension_iterators_.push_back(DimensionIterator<ConstVecIterator> (begin, end));
            // std::cout << *begin << " " << *(end - 1) << std::endl;
            thrust::advance(begin, N_);
            thrust::advance(end, N_);
        }
    }

    // Converts the given device vector of size dim(=len) x n to a vector of vectors of size n(=len) x dim
    std::vector< std::vector<double > > transpose_device_data() const
    {
        // dim x total_number_of_coordinates (len = total_number_of_coordinates)
        // vs. total_number_of_coordinates x dim (len = dim)
        thrust::host_vector<cudaT> host_device_data(*this);

        std::vector< std::vector<double > > transposed_device_data(N_, std::vector<double> (dim_, 0));
        for(auto j = 0; j < dim_; j++) {
            for (auto i = 0; i < N_; i++) {
                transposed_device_data[i][j] = host_device_data[j * N_ + i];
            }
        }
        return transposed_device_data;
    }

private:
    uint dim_;
    uint N_;
    std::vector< DimensionIterator<VecIterator> > dimension_iterators_;
    std::vector < DimensionIterator<ConstVecIterator> > const_dimension_iterators_;
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
