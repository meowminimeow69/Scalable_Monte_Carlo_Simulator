#ifndef PLATEN_01_EXPLICIT_FDM_HPP
#define PLATEN_01_EXPLICIT_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;

template <typename SDEType>
class Platen01ExplicitFDM : public FDMBase<Platen01ExplicitFDM<SDEType>, SDEType> {
public:
	Platen01ExplicitFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	Platen01ExplicitFDM() = delete;
	Platen01ExplicitFDM(const Platen01ExplicitFDM& as) = default;
	Platen01ExplicitFDM& operator=(const Platen01ExplicitFDM& as) = default;
	Platen01ExplicitFDM(Platen01ExplicitFDM&& as) = default;
	Platen01ExplicitFDM& operator=(Platen01ExplicitFDM&& as) = default;
	~Platen01ExplicitFDM() = default;



	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class Platen01ExplicitFDM<GBM>;
extern template class Platen01ExplicitFDM<CEV>;

#endif 