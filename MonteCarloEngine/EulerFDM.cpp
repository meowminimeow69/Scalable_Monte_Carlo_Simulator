#include "EulerFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"

template <typename SDEType>
EulerFDM<SDEType>::EulerFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<EulerFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions){}

template <typename SDEType>
double EulerFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    return xn + (this->sde->Drift(xn, tn) * dt) + (this->sde->Diffusion(xn, tn) * this->dtsqrt * normalVar);
}

template class EulerFDM<GBM>;
template class EulerFDM<CEV>;