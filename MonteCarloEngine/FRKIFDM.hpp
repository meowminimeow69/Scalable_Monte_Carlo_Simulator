#ifndef FRKI_FDM_HPP
#define FRKI_FDM_HPP

#include "FDMBase.hpp"
class GBM;
class CEV;
// similar as C# file
template <typename SDEType>
class FRKIFDM : public FDMBase<FRKIFDM<SDEType>, SDEType> {
private:
	double F1, G1, G2, addedVal, Wincr;
	double sqrk;
public:
	FRKIFDM() = delete;
	FRKIFDM(const FRKIFDM& as) = default;
	FRKIFDM& operator=(const FRKIFDM& as) = default;
	FRKIFDM(FRKIFDM&& as) = default;
	FRKIFDM& operator=(FRKIFDM&& as) = default;
	~FRKIFDM() = default;

	FRKIFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class FRKIFDM<GBM>;
extern template class FRKIFDM<CEV>;

#endif 