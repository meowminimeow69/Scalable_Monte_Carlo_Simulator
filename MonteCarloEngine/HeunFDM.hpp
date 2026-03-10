#ifndef HEUN_FDM_HPP
#define HEUN_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;
// similar to C# file
template <typename SDEType>
class HeunFDM : public FDMBase<HeunFDM<SDEType>, SDEType> {
public:
	HeunFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	HeunFDM() = delete;
	HeunFDM(const HeunFDM& as) = default;
	HeunFDM& operator=(const HeunFDM& as) = default;
	HeunFDM(HeunFDM&& as) = default;
	HeunFDM& operator=(HeunFDM&& as) = default;
	~HeunFDM() = default;
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class HeunFDM<GBM>;
extern template class HeunFDM<CEV>;

#endif 