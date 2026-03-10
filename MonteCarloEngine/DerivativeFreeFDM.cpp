#include "DerivativeFreeFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
DerivativeFreeFDM<SDEType>::DerivativeFreeFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<DerivativeFreeFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {}

template <typename SDEType>
double DerivativeFreeFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
	double dt1 = dt;
	sqrk = std::sqrt(dt1);
	Wincr = sqrk * normalVar;
	F1 = this->sde->Drift(xn, tn);
	G1 = this->sde->Diffusion(xn, tn);
	G2 = this->sde->Diffusion(xn + G1 * sqrk, tn);
	addedVal = 0.5 * (G2 - G1) * (Wincr * Wincr - dt1) / sqrk;
	return xn + (F1 * dt1 + G1 * Wincr + addedVal);
}

template class DerivativeFreeFDM<GBM>;
template class DerivativeFreeFDM<CEV>;