#include"MidpointPredictorCorrectorFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"

template<typename SDEType>
MidpointPredictorCorrectorFDM<SDEType>::MidpointPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b) : FDMBase<MidpointPredictorCorrectorFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions), A(a), B(b), VMid(0.0) {}

template <typename SDEType>
double MidpointPredictorCorrectorFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    VMid = xn + this->sde->Drift(xn, tn) * dt + this->sde->Diffusion(xn, tn) * this->dtsqrt * normalVar;
    double driftTerm = (this->sde->DriftCorrected(A * VMid + (1.0 - A) * xn, tn + dt / 2, B)) * dt;
    double diffusionTerm = (this->sde->Diffusion(B * VMid + (1.0 - B) * xn, tn + dt / 2)) * this->dtsqrt * normalVar;
    return xn + driftTerm + diffusionTerm;
}

template class MidpointPredictorCorrectorFDM<GBM>;
template class MidpointPredictorCorrectorFDM<CEV>;