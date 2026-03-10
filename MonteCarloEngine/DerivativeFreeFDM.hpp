#ifndef DERIVATIVE_FREE_FDM_HPP
#define DERIVATIVE_FREE_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;
// FDM method for FDBMBASE, this one has some variables in it, so it will give some trouble when trying to make it thread safe
// Ideally it should not have any variable and all the advanceimpl can take the process in 
// however inorder to match the C# , I will be building using that so I will follow that
template <typename SDEType>
class DerivativeFreeFDM : public FDMBase<DerivativeFreeFDM<SDEType>, SDEType> {
private:
	double F1, G1, G2, addedVal, Wincr;
	double sqrk;
public:

	DerivativeFreeFDM() = delete;
	DerivativeFreeFDM(const DerivativeFreeFDM& as) = default;
	DerivativeFreeFDM& operator=(const DerivativeFreeFDM& as) = default;
	DerivativeFreeFDM(DerivativeFreeFDM&& as) = default;
	DerivativeFreeFDM& operator=(DerivativeFreeFDM&& as) = default;
	~DerivativeFreeFDM() = default;

	DerivativeFreeFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class DerivativeFreeFDM<GBM>;
extern template class DerivativeFreeFDM<CEV>;

#endif 