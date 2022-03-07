//
// Created by lukas on 12.12.20.
//

#ifndef ONMODELGPU_ON_HPP
#define ONMODELGPU_ON_HPP


#include "link.hpp"
#include <mcmc_simulation/util/random.hpp>


namespace lm_impl {
    namespace link {

        template<typename T, uint N>
        class ON : public Link<T> {
        protected:
            using Link<T>::x_;
        public:
            explicit ON(const std::vector<T> x) {
                if (x.size() != N) {
                    std::cerr << "Invalid size of ON" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                x_ = x;
            }

            explicit ON(const double val) {
                x_ = std::vector<T>(dim(), val);
            }

            ON(const ON<T, N> &on) {
                x_ = on.x_;
            }

            uint dim() const {
                return N;
            }
        };

        template<typename T, uint N>
        T operator*(const ON<T, N> &x, const ON<T, N> &y) {
            T sum = 0;
            for (uint i = 0; i < N; i++)
                sum += x(i) * y(i);
            return sum;
        }

        template<typename T, uint N, typename T2>
        ON<T, N> operator*(const ON<T, N> &x, const T2 &y) {
            ON<T, N> x_(x);
            for (uint i = 0; i < N; i++)
                x_(i) *= y;
            return x_;
        }

        template<typename T, uint N, typename T2>
        ON<T, N> operator*(const T2 &y, const ON<T, N> &x) {
            ON<T, N> x_(x);
            for (uint i = 0; i < N; i++)
                x_(i) *= y;
            return x_;
        }

        template<typename T, uint N, typename T2>
        ON<T, N> operator/(const ON<T, N> &x, const T2 &y) {
            ON<T, N> x_(x);
            for (uint i = 0; i < N; i++)
                x_(i) /= y;
            return x_;
        }

        template<typename T, uint N>
        ON<T, N> operator+(const ON<T, N> &a, const ON<T, N> &b) {
            auto val(a);
            val += b;
            return val;
        }
    }

}

namespace std {
    template<typename T, uint N>
    std::string to_string(lm_impl::link::ON<T, N> x) {
        std::string conf = "";
        for (uint j = 0; j < x.dim(); j++)
            conf += std::to_string(x(j)) + " ";
        conf = conf.substr(0, conf.size() - 1);
        return conf;
    }

    template<typename T, uint N>
    double fabs(lm_impl::link::ON<T, N> arg) {
        return std::sqrt(arg * arg);
    }

    template<uint N>
    double fabs(lm_impl::link::ON<std::complex<double>, N> arg) {
        std::pair<double, double> norm(0, 0);
        for (uint j = 0; j < arg.dim(); j++)
        {
            norm.first += arg(j).real() * arg(j).real();
            norm.second += arg(j).imag() * arg(j).imag();
        }
        return std::sqrt(norm.first + norm.second);
    }
}

#endif //ONMODELGPU_ON_HPP