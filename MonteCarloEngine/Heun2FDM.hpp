#ifndef HEUN_2_FDM_HPP
#define HEUN_2_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;
// similar to the C# 
template <typename SDEType>
class Heun2FDM : public FDMBase<Heun2FDM<SDEType>, SDEType> {
private:
	double F1, F2, G1, G2, addedVal, Wincr, tmp;
	double sqrk;
	double F(double  x, double  t);
public:
	Heun2FDM() = delete;
	Heun2FDM(const Heun2FDM& as) = default;
	Heun2FDM& operator=(const Heun2FDM& as) = default;
	Heun2FDM(Heun2FDM&& as) = default;
	Heun2FDM& operator=(Heun2FDM&& as) = default;
	~Heun2FDM() = default;

	Heun2FDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class Heun2FDM<GBM>;
extern template class Heun2FDM<CEV>;

#endif 