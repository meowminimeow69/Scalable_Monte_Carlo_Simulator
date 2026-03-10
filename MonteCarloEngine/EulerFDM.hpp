#ifndef Euler_FDM_HPP
#define Euler_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;
// eurler fdm
template <typename SDEType>
class EulerFDM : public FDMBase<EulerFDM<SDEType>, SDEType> {
public:
	EulerFDM() = delete;
	EulerFDM(const EulerFDM& as) = default;
	EulerFDM& operator=(const EulerFDM& as) = default;
	EulerFDM(EulerFDM&& as) = default;
	EulerFDM& operator=(EulerFDM&& as) = default;
	~EulerFDM() = default;

	EulerFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) ;
};

extern template class EulerFDM<GBM>;
extern template class EulerFDM<CEV>;

#endif // !Euler_FDM_HPP

