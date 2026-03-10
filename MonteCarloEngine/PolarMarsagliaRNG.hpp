#ifndef POLAR_MARSAGLIA_RNG_HPP
#define POLAR_MARSAGLIA_RNG_HPP

#include"RNGBase.hpp"

class PolarMarsagliaRNG : public RNGBase<PolarMarsagliaRNG> {
private:
	std::mt19937 engine_;
	std::uniform_real_distribution<double> dist_{ 0.0, 1.0 };
public:
	PolarMarsagliaRNG();

	PolarMarsagliaRNG(const PolarMarsagliaRNG& as) = delete;
	PolarMarsagliaRNG& operator=(const PolarMarsagliaRNG& as) = delete;
	PolarMarsagliaRNG(PolarMarsagliaRNG&& as) = default;
	PolarMarsagliaRNG& operator=(PolarMarsagliaRNG&& as) = default;
	~PolarMarsagliaRNG() = default;

	double generateRnImpl();
};


#endif 
