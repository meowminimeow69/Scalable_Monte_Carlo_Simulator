#ifndef DISCRETE_MILSTEIN_FDM_HPP
#define DISCRETE_MILSTEIN_FDM_HPP

#include"FDMBase.hpp"
class GBM;
class CEV;
// CRTP version of Discrete Milseten
template<typename SDEType>
class DiscreteMilsteinFDM : public FDMBase<DiscreteMilsteinFDM<SDEType>, SDEType> {
public:
	DiscreteMilsteinFDM() = delete;
	DiscreteMilsteinFDM(const DiscreteMilsteinFDM& as) = default;
	DiscreteMilsteinFDM& operator=(const DiscreteMilsteinFDM& as) = default;
	DiscreteMilsteinFDM(DiscreteMilsteinFDM&& as) = default;
	DiscreteMilsteinFDM& operator=(DiscreteMilsteinFDM&& as) = default;
	~DiscreteMilsteinFDM() = default;

	DiscreteMilsteinFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class DiscreteMilsteinFDM<GBM>;
extern template class DiscreteMilsteinFDM<CEV>;


#endif 
