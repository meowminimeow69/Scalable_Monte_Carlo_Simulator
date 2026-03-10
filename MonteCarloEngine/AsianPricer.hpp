#ifndef ASIAN_PRICER_HPP
#define ASIAN_PRICER_HPP

#include "PricerBase.hpp"

template <typename PayoffFunc, typename DiscountFunc>
class AsianPricer : public PricerBase<AsianPricer<PayoffFunc, DiscountFunc>, PayoffFunc, DiscountFunc>{
public:
	AsianPricer(PayoffFunc   payoff,DiscountFunc discounter) : PricerBase<AsianPricer<PayoffFunc, DiscountFunc>,PayoffFunc, DiscountFunc>(payoff, discounter){}

	AsianPricer() = delete;
	AsianPricer(const AsianPricer& as) = default;
	AsianPricer& operator=(const AsianPricer& as) = default;
	AsianPricer(AsianPricer&& as) = default;
	AsianPricer& operator=(AsianPricer&& as) = default;
	~AsianPricer() = default;

	double ProcessPathImpl(const std::vector<double>& path) const {
		const std::size_t N = path.size() - 1;
		double average = 0.0;

		for (std::size_t i = 1; i <= N; ++i) average += path[i];
		
		average /= static_cast<double>(N);
		return average;
	}
};


#endif 