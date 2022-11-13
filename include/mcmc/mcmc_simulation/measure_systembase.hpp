#ifndef MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
#define MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP

#include <vector>

#include <param_helper/params.hpp>
#include <mcmc/mcmc_simulation/systembase.hpp>
#include <mcmc/mcmc_simulation/measure_policy.hpp>

namespace mcmc::simulation {
    template<typename Derived>
    class MeasureSystemBase : public SystemBase<Derived> {
    public:
        MeasureSystemBase(const json params) : SystemBase<Derived>(params)
        {}

        void initialize_measurements(std::string starting_mode, uint rep=1)
        {}

        // Predefined measure method
        virtual std::vector<std::variant<int, double, std::string>> perform_measurements() {
            std::vector<std::variant<int, double, std::string>> results;
            for (auto const &element: measurements_) {
                results.push_back(element->measure(this->systembase()));
            }
            return results;
        }

        void finalize_measurements(std::string starting_mode, uint rep=1)
        {}

        /* By overriding this function one can also add custom generated
        measures */
        virtual void generate_measures(const std::vector<std::string>& measure_names)
        {
            measurements_.clear();
            auto common_defined_measures = generate_systembase_measures(measure_names);
            this->concat_measures(common_defined_measures);

            this->sort_measures();
        }

    protected:
        virtual std::vector< std::unique_ptr<measures::Measure<Derived>> > generate_systembase_measures(const std::vector<std::string>& measure_names);

        void concat_measures(std::vector<std::unique_ptr<measures::Measure<Derived>>> &measurements);

        void sort_measures();

        // Stores all measure objects - these objects need to be created by the derived class by calling generate_measures in void initialize()
        std::vector<std::unique_ptr<measures::Measure<Derived>>> measurements_;
    };

    template<typename Derived>
    std::vector< std::unique_ptr<measures::Measure<Derived>> > MeasureSystemBase<Derived>::generate_systembase_measures(const std::vector<std::string>& measure_names)
    {
        std::vector<std::unique_ptr<measures::Measure<Derived>>> measurements{};
        for(auto &measure_name : measure_names) {
            if (measure_name == "Mean")
                measurements.push_back(std::make_unique<measures::Mean<Derived>>());
            else if (measure_name == "AbsMean")
                measurements.push_back(std::make_unique<measures::AbsMean<Derived>>());
            else if (measure_name == "Abs")
                measurements.push_back(std::make_unique<measures::Abs<Derived>>());
            else if (measure_name == "Variance")
                measurements.push_back(std::make_unique<measures::Variance<Derived>>());
            else if (measure_name == "SecondMoment")
                measurements.push_back(std::make_unique<measures::SecondMoment<Derived>>());
            else if (measure_name == "FourthMoment")
                measurements.push_back(std::make_unique<measures::FourthMoment<Derived>>());
            else if (measure_name == "Config")
                measurements.push_back(std::make_unique<measures::Config<Derived>>());
        }
        return measurements;
    }

    template<typename Derived>
    void  MeasureSystemBase<Derived>::concat_measures(
            std::vector<std::unique_ptr<measures::Measure<Derived>>> &measurements) {
        std::move(begin(measurements), end(measurements), std::inserter(measurements_, end(measurements_)));
    }

    template<typename Derived>
    void MeasureSystemBase<Derived>::sort_measures()
    {
        for(auto i = 0; i < measurements_.size(); i++)
        {
            auto &measure_name = this->measures_[i];
            if(measurements_[i]->name() != measure_name)
            {
                for(auto j = i + 1; j < measurements_.size(); j++)
                {
                    if(measurements_[j]->name() == measure_name)
                    {
                        std::swap(measurements_[i], measurements_[j]);
                    }
                }
            }
        }
    }
}

#endif //MCMCSIMULATIONLIB_SYSTEMBASE_MEASURES_HPP
