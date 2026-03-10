#include"FittedMidpointPredictorCorrectorFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
FittedMidpointPredictorCorrectorFDM<SDEType>::FittedMidpointPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b) : FDMBase<FittedMidpointPredictorCorrectorFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions), A(a), B(b), VMid(0.0) {}

template <typename SDEType>
double FittedMidpointPredictorCorrectorFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double aFit = (std::exp(0.08 * dt) - 1.0) / dt;
    VMid = xn + aFit * xn * dt + this->sde->Diffusion(xn, tn) * this->dtsqrt * normalVar;
    double driftTerm = (this->sde->DriftCorrected(A * VMid + (1.0 - A) * xn, tn + dt / 2, B)) * dt;
    double diffusionTerm = (this->sde->Diffusion(B * VMid + (1.0 - B) * xn, tn + dt / 2)) * this->dtsqrt * normalVar;
    return xn + driftTerm + diffusionTerm;
}

template class FittedMidpointPredictorCorrectorFDM<GBM>;
template class FittedMidpointPredictorCorrectorFDM<CEV>;