#include <vector>
#include <iostream>

namespace mcmc {
    namespace util {
        /** @brief Returns evenly spaced numbers over a specified interval, where ``min`` and ``max`` are included.
         *
         * @param min Lower bound of the interval
         * @param max Upper bound of the interval
         * @param num Number of numbers in the interval
         * @returns A vector with evenly spaced numbers over the specified interval.
         */
        template<typename T>
        std::vector<T> linspace(T min, T max, uint num=1)
        {
            double interval;
            if(num == 1)
                interval = min;
            else
                interval = (max - min) / (num - 1);
            std::vector<T> intervals(num, 0.0);
            for( uint i = 0; i < num; i++)
                intervals[i] = min + i * interval;
            return intervals;
        }
    }
}