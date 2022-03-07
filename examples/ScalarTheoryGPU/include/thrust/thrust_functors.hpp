//
// Created by lukas on 14.03.19.
//

#ifndef PROJECT_THRUST_FUNCTORS_HPP
#define PROJECT_THRUST_FUNCTORS_HPP

#include <thrust/device_vector.h>
#include <thrust/reduce.h>
#include <thrust/gather.h>
#include <thrust/scan.h>
#include <thrust/fill.h>
#include <thrust/copy.h>

#include <iterator>
#include <iostream>
// from https://github.com/thrust/thrust/blob/master/examples/repeated_range.cu

template <typename Iterator>
class repeated_range
{
public:

    typedef typename thrust::iterator_difference<Iterator>::type difference_type;

    struct repeat_functor : public thrust::unary_function<difference_type,difference_type>
    {
        difference_type repeats;

        repeat_functor(difference_type repeats)
                : repeats(repeats) {}

        __host__ __device__
        difference_type operator()(const difference_type& i) const
        {
            return i / repeats;
        }
    };

    typedef typename thrust::counting_iterator<difference_type>                   CountingIterator;
    typedef typename thrust::transform_iterator<repeat_functor, CountingIterator> TransformIterator;
    typedef typename thrust::permutation_iterator<Iterator,TransformIterator>     PermutationIterator;

    // type of the repeated_range iterator
    typedef PermutationIterator iterator;

    // construct repeated_range for the range [first,last)
    repeated_range(Iterator first, Iterator last, difference_type repeats)
            : first(first), last(last), repeats(repeats) {}

    iterator begin(void) const
    {
        return PermutationIterator(first, TransformIterator(CountingIterator(0), repeat_functor(repeats)));
    }

    iterator end(void) const
    {
        return begin() + repeats * (last - first);
    }

protected:
    Iterator first;
    Iterator last;
    difference_type repeats;
};


// from https://github.com/thrust/thrust/blob/master/examples/tiled_range.cu

template <typename Iterator>
class tiled_range
{
public:

    typedef typename thrust::iterator_difference<Iterator>::type difference_type;

    struct tile_functor : public thrust::unary_function<difference_type,difference_type>
    {
        difference_type tile_size;

        tile_functor(difference_type tile_size)
                : tile_size(tile_size) {}

        __host__ __device__
        difference_type operator()(const difference_type& i) const
        {
            return i % tile_size;
        }
    };

    typedef typename thrust::counting_iterator<difference_type>                   CountingIterator;
    typedef typename thrust::transform_iterator<tile_functor, CountingIterator>   TransformIterator;
    typedef typename thrust::permutation_iterator<Iterator,TransformIterator>     PermutationIterator;

    // type of the tiled_range iterator
    typedef PermutationIterator iterator;

    // construct repeated_range for the range [first,last)
    tiled_range(Iterator first, Iterator last, difference_type tiles)
            : first(first), last(last), tiles(tiles) {}

    iterator begin(void) const
    {
        return PermutationIterator(first, TransformIterator(CountingIterator(0), tile_functor(last - first)));
    }

    iterator end(void) const
    {
        return begin() + tiles * (last - first);
    }

protected:
    Iterator first;
    Iterator last;
    difference_type tiles;
};


// from https://github.com/thrust/thrust/blob/master/examples/expand.cu

template <typename InputIterator1,
        typename InputIterator2,
        typename OutputIterator>
OutputIterator expand(InputIterator1 first1,
                      InputIterator1 last1,
                      InputIterator2 first2,
                      OutputIterator output)
{
    typedef typename thrust::iterator_difference<InputIterator1>::type difference_type;

    difference_type input_size  = thrust::distance(first1, last1);
    difference_type output_size = thrust::reduce(first1, last1);

#ifndef GPU
    // scan the counts to obtain output offsets for each input element
    thrust::host_vector<difference_type> output_offsets(input_size, 0);
    thrust::exclusive_scan(first1, last1, output_offsets.begin());

    // scatter the nonzero counts into their corresponding output positions
    thrust::host_vector<difference_type> output_indices(output_size, 0);
    thrust::scatter_if
            (thrust::counting_iterator<difference_type>(0),
             thrust::counting_iterator<difference_type>(input_size),
             output_offsets.begin(),
             first1,
             output_indices.begin());
#else
    // scan the counts to obtain output offsets for each input element
    thrust::device_vector<difference_type> output_offsets(input_size, 0);
    thrust::exclusive_scan(first1, last1, output_offsets.begin());

    // scatter the nonzero counts into their corresponding output positions
    thrust::device_vector<difference_type> output_indices(output_size, 0);
    thrust::scatter_if
            (thrust::counting_iterator<difference_type>(0),
             thrust::counting_iterator<difference_type>(input_size),
             output_offsets.begin(),
             first1,
             output_indices.begin());
#endif

    // compute max-scan over the output indices, filling in the holes
    thrust::inclusive_scan
            (output_indices.begin(),
             output_indices.end(),
             output_indices.begin(),
             thrust::maximum<difference_type>());

    // gather input values according to index array (output = first2[output_indices])
    OutputIterator output_end = output; thrust::advance(output_end, output_size);
    thrust::gather(output_indices.begin(),
                   output_indices.end(),
                   first2,
                   output);

    // return output + output_size
    thrust::advance(output, output_size);
    return output;
}


// from https://github.com/thrust/thrust/blob/master/examples/stream_compaction.cu

template <typename Iterator>
void print_range(const std::string& name, Iterator first, Iterator last)
{
    typedef typename std::iterator_traits<Iterator>::value_type T;

    std::cout << name << ": ";
    thrust::copy(first, last, std::ostream_iterator<T>(std::cout, " "));
    std::cout << "\n";
}

template <typename Iterator>
void print_range_in_os(Iterator first, Iterator last, std::ofstream &os)
{
    typedef typename std::iterator_traits<Iterator>::value_type T;
    thrust::copy(first, last, std::ostream_iterator<T>(os, " "));
}


// from https://github.com/thrust/thrust/blob/master/examples/sum_rows.cu


// convert a linear index to a row index
template <typename T>
struct linear_index_to_row_index : public thrust::unary_function<T,T>
{
    T C; // number of columns

    __host__ __device__
    linear_index_to_row_index(T C) : C(C) {}

    __host__ __device__
    T operator()(T i)
    {
        return i / C;
    }
};


#endif //PROJECT_THRUST_FUNCTORS_HPP
