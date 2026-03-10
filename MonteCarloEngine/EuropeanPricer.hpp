#ifndef EUROPEAN_PRICER_HPP
#define EUROPEAN_PRICER_HPP

#include "PricerBase.hpp"

// european pricr
template <typename PayoffFunc, typename DiscountFunc>
class EuropeanPricer : public PricerBase<EuropeanPricer<PayoffFunc, DiscountFunc>, PayoffFunc, DiscountFunc> {
public:
    EuropeanPricer(PayoffFunc payoff, DiscountFunc discounter)
        : PricerBase<EuropeanPricer<PayoffFunc, DiscountFunc>, PayoffFunc, DiscountFunc>(payoff, discounter) {}

    EuropeanPricer() = delete;
    EuropeanPricer(const EuropeanPricer& as) = default;
    EuropeanPricer& operator=(const EuropeanPricer& as) = default;
    EuropeanPricer(EuropeanPricer&& as) = default;
    EuropeanPricer& operator=(EuropeanPricer&& as) = default;
    ~EuropeanPricer() = default;

    double ProcessPathImpl(const std::vector<double>& path) const {
        return this->m_payoff(path.back()); // it is path terminal
    }

    double FinalPrice(double totalSum, int NSim) const {
        return (totalSum / static_cast<double>(NSim)) * this->DiscountFactor();
    }
};

#endif