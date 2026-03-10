#include "Heun2FDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
Heun2FDM<SDEType>::Heun2FDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<Heun2FDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {}

template<typename SDEType>
double Heun2FDM<SDEType>::F(double  x, double  t) {
	return this->sde->Drift(x, t) - 0.5 * this->sde->DiffusionDerivative(x, t) * this->sde->Diffusion(x, t);
}

template <typename SDEType>
double Heun2FDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double dt1 = dt;
    sqrk = std::sqrt(dt1);
    Wincr = sqrk * normalVar;
    F1 = F(xn, tn);
    G1 = this->sde->Diffusion(xn, tn);
    tmp = xn + F1 * dt1 + G1 * Wincr;
    F2 = F(tmp, tn);
    G2 = this->sde->Diffusion(tmp, tn);
    return xn + 0.5 * (F1 + F2) * dt1 + 0.5 * (G1 + G2) * Wincr;
}

template class Heun2FDM<GBM>;
template class Heun2FDM<CEV>;