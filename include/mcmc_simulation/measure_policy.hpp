#ifndef MEASUREPOLICY_HPP
#define MEASUREPOLICY_HPP

#include <iostream>
#include <complex>

// These need to be implemented somewhere else!
/* #include "../u1.hpp"
#include "../su2.hpp"
#include "../nvec.hpp"
#include "../nvec_complex.hpp" */

/* namespace std
{
    template<typename T>
    double fabs(const U1<T> x)
    {
        return std::fabs(x(0));
    }

    template<typename T>
    double fabs(const SU2<T> x)
    {
        // ToDo!! Incorrect return type for SU2
        return std::fabs(x(0));
    }

    template<typename T, uint N>
    double fabs(NVec<T, N> arg)
    {
        return 0.0;
    }

    template<typename T, uint N, std::vector<bool> &real_>
    double fabs(NVecComplex<T, N, real_> arg)
    {
        return 0.0;
    }
} */


template<typename T>
struct TypePolicy {
public:
    static double realv(const T state) {
        return state;
    }

    static double imagv(const T state) {
        return state;
    }

    static std::string conf(const T state) {
        return std::to_string(state) + " ";
    }
};

template<>
struct TypePolicy< std::complex<double> > {
public:
    static double realv(const std::complex<double> state) {
        return state.real();
    }

    static double imagv(const std::complex<double> state) {
        return state.imag();
    }

    static std::string conf(const std::complex<double> state) {
        return std::to_string(state.real()) + " " + std::to_string(state.imag()) + " ";
    }
};

template<>
struct TypePolicy< const std::complex<double> > {
public:
    static double realv(const std::complex<double> state) {
        return state.real();
    }

    static double imagv(const std::complex<double> state) {
        return state.imag();
    }

    static std::string conf(const std::complex<double> state) {
        return std::to_string(state.real()) + " " + std::to_string(state.imag()) + " ";
    }
};

/* template<>
struct TypePolicy< U1<std::complex<double> > > {
public:
    static double realv(const U1<std::complex<double> > state) {
        return state(0).real();
    }

    static double imagv(const U1<std::complex<double> > state) {
        return state(0).imag();
    }

    static std::string conf(const U1<std::complex<double> > state) {
        return " ";
    }
};

template<>
struct TypePolicy< U1<const std::complex<double> > > {
public:
    static double realv(const U1<std::complex<double> > state) {
        return state(0).real();
    }

    static double imagv(const U1<std::complex<double> > state) {
        return state(0).imag();
    }

    static std::string conf(const U1<std::complex<double> > state) {
        return " ";
    }
};

template<>
struct TypePolicy< const U1<std::complex<double> > > {
public:
    static double realv(const U1<std::complex<double> > state) {
        return state(0).real();
    }

    static double imagv(const U1<std::complex<double> > state) {
        return state(0).imag();
    }

    static std::string conf(const U1<std::complex<double> > state) {
        return " ";
    }
};

template<>
struct TypePolicy< const U1<const std::complex<double> > > {
public:
    static double realv(const U1<std::complex<double> > state) {
        return state(0).real();
    }

    static double imagv(const U1<std::complex<double> > state) {
        return state(0).imag();
    }

    static std::string conf(const U1<std::complex<double> > state) {
        return " ";
    }
};

template<>
struct TypePolicy< SU2<double> > {
public:
    static double realv(const SU2<double> state) {
        return 0.0; // state(0).real();
    }

    static double imagv(const SU2<double> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const SU2<double> state) {
        return " ";
    }
};

template<>
struct TypePolicy< SU2<const double> > {
public:
    static double realv(const SU2<double> state) {
        return 0.0; //state(0).real();
    }

    static double imagv(const SU2<double> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const SU2<double> state) {
        return " ";
    }
};

template<>
struct TypePolicy< const SU2<double> > {
public:
    static double realv(const SU2<double> state) {
        return 0.0; //state(0).real();
    }

    static double imagv(const SU2<double> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const SU2<double> state) {
        return " ";
    }
};

template<>
struct TypePolicy< const SU2<const double> > {
public:
    static double realv(const SU2<double> state) {
        return 0.0; //state(0).real();
    }

    static double imagv(const SU2<double> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const SU2<double> state) {
        return " ";
    }
};

template<typename T, uint N>
struct TypePolicy< NVec<T,N> > {
public:
    static double realv(const NVec<T,N> state) {
        return state.reduce();
    }

    static double imagv(const NVec<T,N> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const NVec<T, N> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N>
struct TypePolicy< NVec<const T, N> > {
public:
    static double realv(const NVec<T,N> state) {
        return state.reduce();
    }

    static double imagv(const NVec<T,N> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const NVec<T, N> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N>
struct TypePolicy< const NVec<T,N> > {
public:
    static double realv(const NVec<T,N> state) {
        return state.reduce();
    }

    static double imagv(const NVec<T,N> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const NVec<T, N> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N>
struct TypePolicy< const NVec<const T, N> > {
public:
    static double realv(const NVec<T,N> state) {
        return state.reduce();
    }

    static double imagv(const NVec<T,N> state) {
        return 0.0; //state(0).imag();
    }

    static std::string conf(const NVec<T, N> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N, std::vector<bool> &real_>
struct TypePolicy< NVecComplex<T,N,real_> > {
public:
    static double realv(const NVecComplex<T,N,real_> state) {
        return state.reduce().real();
    }

    static double imagv(const NVecComplex<T,N,real_> state) {
        return state.reduce().imag();
    }

    static std::string conf(const NVecComplex<T,N,real_> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N, std::vector<bool> &real_>
struct TypePolicy< NVecComplex<const T,N,real_> > {
public:
    static double realv(const NVecComplex<T,N,real_> state) {
        return state.reduce().real();
    }

    static double imagv(const NVecComplex<T,N,real_> state) {
        return state.reduce().imag();
    }

    static std::string conf(const NVec<T, N> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N, std::vector<bool> &real_>
struct TypePolicy< const NVecComplex<T,N,real_> > {
public:
    static double realv(const NVecComplex<T,N,real_> state) {
        return state.reduce().real();
    }

    static double imagv(const NVecComplex<T,N,real_> state) {
        return state.reduce().imag();
    }

    static std::string conf(const NVecComplex<T,N,real_> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
};

template<typename T, uint N, std::vector<bool> &real_>
struct TypePolicy< const NVecComplex<const T,N,real_> > {
public:
    static double realv(const NVecComplex<T,N,real_> state) {
        return state.reduce().real();
    }

    static double imagv(const NVecComplex<T,N,real_> state) {
        return state.reduce().imag();
    }

    static std::string conf(const NVecComplex<T,N,real_> state) {
        std::string conf = "";
        for(auto j = 0; j < state.dim(); j++)
            conf += std::to_string(state(j)) + " ";
        conf = conf.substr(0, conf.size() -1);
        return conf;
    }
}; */


template<typename SB>
struct MeasurePolicy {
    virtual std::string measure(const SB &system) = 0;
    virtual std::string name() = 0;
};

template<typename SB>
struct MeasureMeanPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        auto sum = decltype(system[0]){0};
        for(auto i = 0; i < system.size(); i++)
            sum += system[i];
        return std::to_string(TypePolicy<decltype(sum)>::realv(sum)*1.0/system.size());
    }

    std::string name()
    {
        return "Mean";
    }
};

template<typename SB>
struct MeasureMeanImagPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        auto sum = decltype(system[0]){0};
        for(auto i = 0; i < system.size(); i++)
            sum += system[i];
        return std::to_string(TypePolicy<decltype(sum)>::imagv(sum)*1.0/system.size());
    }

    std::string name()
    {
        return "MeanImag";
    }
};

template<typename SB>
struct MeasureAbsPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        double sum = 0.0;
        for(auto i = 0; i < system.size(); i++)
            sum += std::fabs(system[i]);
        return std::to_string(sum*1.0/system.size());
    }

    std::string name()
    {
        return "Abs";
    }
};

template<typename SB>
struct MeasureAbsMeanPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB & system) override {
        auto sum = decltype(system[0]){0};
        for(auto i = 0; i < system.size(); i++)
            sum += system[i];
        return std::to_string(abs(TypePolicy<decltype(sum)>::realv(sum))*1.0/system.size());
    }

    std::string name()
    {
        return "AbsMean";
    }
};

template<typename SB>
struct MeasureStdPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        auto sum = decltype(system[0]){0};
        //sum += 1;
        //std::cout << a << " " << sum << std::endl;
        return std::to_string(TypePolicy<decltype(sum)>::realv(system[0]));
    }

    std::string name()
    {
        return "Std";
    }
};

template<typename SB>
struct MeasureSecondMomentPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        auto sum = decltype(system[0]){0};
        /* for(auto i = 0; i < system.size(); i++)
            sum += pow(system[i], 2); */
        return std::to_string(TypePolicy<decltype(sum)>::realv(sum)*1.0/system.size());
    }

    std::string name()
    {
        return "SecondMoment";
    }
};

template<typename SB>
struct MeasureFourthMomentPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        auto sum = decltype(system[0]){0};
        // for(auto i = 0; i < system.size(); i++)
        //     sum += pow(system[i], 4);
        return std::to_string(TypePolicy<decltype(sum)>::realv(sum)*1.0/system.size());
    }

    std::string name()
    {
        return "FourthMoment";
    }
};

template<typename SB>
struct MeasureConfigPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        std::string config = "";
        for(auto i = 0; i < system.size(); i++)
            config += std::to_string(TypePolicy<decltype(system[i])>::realv(system[i])) + " ";
        return config.substr(0, config.size()-1);
    }

    std::string name()
    {
        return "Config";
    }
};

template<typename SB>
struct MeasureComplexConfigPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        std::string config;
        for(auto i = 0; i < system.size(); i++) {
            config += TypePolicy<decltype(system[i])>::conf(system[i]);
        }
        return config;
    }

    std::string name()
    {
        return "ComplexConfig";
    }
};

template<typename SB>
struct MeasureRepaConfigPolicy : public MeasurePolicy<SB> {
    std::string measure(const SB &system) override {
        std::string config;
        for(auto i = 0; i < system.size(); i++) {
            config += TypePolicy<decltype(system[i])>::conf(system[i]);
        }
        return config;
    }

    std::string name()
    {
        return "RepaConfig";
    }
};

template<typename SB>
std::unique_ptr<MeasurePolicy<SB>> unique_measure_factory(const std::string& measure) {
    if(measure == "Mean")
        return std::make_unique<MeasureMeanPolicy<SB>>();
    if(measure == "MeanImag")
        return std::make_unique<MeasureMeanImagPolicy<SB>>();
    else if(measure == "AbsMean")
        return std::make_unique<MeasureAbsMeanPolicy<SB>>();
    else if(measure == "Abs")
        return std::make_unique<MeasureAbsPolicy<SB>>();
    else if(measure == "Std")
        return std::make_unique<MeasureStdPolicy<SB>>();
    else if(measure == "SecondMoment")
        return std::make_unique<MeasureSecondMomentPolicy<SB>>();
    else if(measure == "FourthMoment")
        return std::make_unique<MeasureFourthMomentPolicy<SB>>();
    else if(measure == "Config")
        return std::make_unique<MeasureConfigPolicy<SB>>();
    else if(measure == "ComplexConfig")
        return std::make_unique<MeasureComplexConfigPolicy<SB>>();
    else if(measure == "RepaConfig")
        return std::make_unique<MeasureRepaConfigPolicy<SB>>();
    else
    {
        std::cout << "Measure not known: " << measure << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#endif
