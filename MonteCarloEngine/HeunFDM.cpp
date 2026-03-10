#include "HeunFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
HeunFDM<SDEType>::HeunFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<HeunFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {}

template <typename SDEType>
double HeunFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double a = this->sde->Drift(xn, tn);
    double b = this->sde->Diffusion(xn, tn);
    double suppValue = xn + a * dt + b * std::sqrt(dt) * normalVar;

    return xn + 0.5 * (this->sde->Drift(suppValue, tn) + a) * dt + 0.5 * (this->sde->Diffusion(suppValue, tn) + b) * std::sqrt(dt) * normalVar;
}

template class HeunFDM<GBM>;
template class HeunFDM<CEV>;