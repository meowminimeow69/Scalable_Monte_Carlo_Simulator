#ifndef PRICER_BASE_HPP
#define PRICER_BASE_HPP

#include<vector>


template <typename Derived, typename PayoffFunc, typename DiscountFunc>
class PricerBase {
protected:
	PayoffFunc m_payoff;
	DiscountFunc m_discounter;
public:
	PricerBase(PayoffFunc payoff, DiscountFunc discounter): m_payoff(payoff), m_discounter(discounter) {}
	
	PricerBase() = delete;
	PricerBase(const PricerBase& as) = default;
	PricerBase& operator=(const PricerBase& as) = default;
	PricerBase(PricerBase&& as) = default;
	PricerBase& operator=(PricerBase&& as) = default;
	~PricerBase() = default;


	double ProcessPath(const std::vector<double>& path) const {
		return static_cast<const Derived*>(this)->ProcessPathImpl(path);
	}
	// remains consistant across all option pricers
	double DiscountFactor() const {
		return m_discounter();
	}
};

#endif 
