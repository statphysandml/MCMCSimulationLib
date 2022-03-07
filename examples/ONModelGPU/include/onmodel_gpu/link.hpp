//
// Created by lukas on 04.11.19.
//

#ifndef ONMODELGPU_LINK_HPP
#define ONMODELGPU_LINK_HPP


#include <iomanip>
#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <algorithm>

#include <mcmc_simulation/util/random.hpp>


namespace lm_impl {
    namespace link {

        template<typename T>
        class Link {
        protected:
            std::vector<T> x_;
        public:
            void Print() const;

            T operator()(int i) const;

            T &operator()(int i);

            virtual Link &operator+=(const Link &x);

            virtual Link &operator-=(const Link &x);

            virtual Link &operator*=(const double &x);

            virtual Link &operator/=(const double &x);

            virtual Link &adjungate();
        };


        template<typename T>
        void Link<T>::Print() const {
            std::cout << "Link = " << x_[0];
            for (auto i = 1; i < x_.size(); i++)
                std::cout << ", " << x_[i];
            std::cout << std::endl;
        }

        template<typename T>
        T Link<T>::operator()(int i) const {
            return x_[i];
        }

        template<typename T>
        T &Link<T>::operator()(int i) {
            return x_[i];
        }

        template<typename T>
        Link<T> &Link<T>::operator+=(const Link<T> &x) {
            for (size_t i = 0; i < x_.size(); i++)
                x_[i] += x(i);
            return *this;
        }

        template<typename T>
        Link<T> &Link<T>::operator-=(const Link<T> &x) {
            for (size_t i = 0; i < x_.size(); i++)
                x_[i] -= x(i);
            return *this;
        }

        template<typename T>
        Link<T> &Link<T>::operator*=(const double &x) {
            for (size_t i = 0; i < x_.size(); i++)
                x_[i] *= x;
            return *this;
        }

        template<typename T>
        Link<T> &Link<T>::operator/=(const double &x) {
            for (size_t i = 0; i < x_.size(); i++)
                x_[i] /= x;
            return *this;
        }

        template<typename T>
        Link<T> &Link<T>::adjungate() {
            return *this;
        }

    }
}

#endif //ONMODELGPU_LINK_HPP
