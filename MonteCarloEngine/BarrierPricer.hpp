#ifndef BARRIER_PRICER_HPP
#define BARRIER_PRICER_HPP

#include "PricerBase.hpp"
/*
Barrier options pricing is dervied from Pricer baser using CRTP
*/
template <typename PayoffFunc, typename DiscountFunc>
class BarrierPricer : public PricerBase<BarrierPricer<PayoffFunc, DiscountFunc>, PayoffFunc, DiscountFunc> {
private:
    double m_barrier;
    double m_rebate;

public:
    BarrierPricer(PayoffFunc payoff, DiscountFunc discounter, double barrier, double rebate = 0.0) : PricerBase<BarrierPricer<PayoffFunc, DiscountFunc>, PayoffFunc, DiscountFunc>(payoff, discounter),
        m_barrier(barrier), m_rebate(rebate) {}

    BarrierPricer() = delete;
    BarrierPricer(const BarrierPricer& as) = default;
    BarrierPricer& operator=(const BarrierPricer& as) = default;
    BarrierPricer(BarrierPricer&& as) = default;
    BarrierPricer& operator=(BarrierPricer&& as) = default;
    ~BarrierPricer() = default;


    double ProcessPathImpl(const std::vector<double>& path) const {
        bool crossed = false;

        for (double price : path) {
            if (price >= m_barrier) {
                crossed = true;
                break;
            }
        }

        if (!crossed) {
            return this->m_payoff(path.back());
        }
        else {
            return m_rebate;
        }
    }
};

#endif