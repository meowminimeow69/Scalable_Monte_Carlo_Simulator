#include "Platen01ExplicitFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
Platen01ExplicitFDM<SDEType>::Platen01ExplicitFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<Platen01ExplicitFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions){}

template <typename SDEType>
double Platen01ExplicitFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double b = this->sde->Diffusion(xn, tn);
    double drift_Strat = this->sde->Drift(xn, tn) - 0.5 * b * this->sde->DiffusionDerivative(xn, tn);
    double suppValue = xn + drift_Strat * dt + b * std::sqrt(dt);

    return xn + drift_Strat * dt + b * std::sqrt(dt) * normalVar + 0.5 * std::sqrt(dt) * (this->sde->Diffusion(suppValue, tn) - b) * normalVar * normalVar;
}

template class Platen01ExplicitFDM<GBM>;
template class Platen01ExplicitFDM<CEV>;