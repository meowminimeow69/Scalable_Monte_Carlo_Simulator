#include"ModifiedPredictorCorrectorFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"

template<typename SDEType>
ModifiedPredictorCorrectorFDM<SDEType>::ModifiedPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b) : FDMBase<ModifiedPredictorCorrectorFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions), A(a), B(b), VMid(0.0){}

template <typename SDEType>
double ModifiedPredictorCorrectorFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    VMid = xn + this->sde->Drift(xn, tn) * dt + this->sde->Diffusion(xn, tn) * this->dtsqrt * normalVar;
    double  driftTerm = (A * this->sde->DriftCorrected(VMid, tn + dt, B) + ((1.0 - A) * this->sde->DriftCorrected(xn, tn, B))) * dt;
    double  diffusionTerm = (B * this->sde->Diffusion(VMid, tn + dt) + ((1.0 - B) * this->sde->Diffusion(xn, tn))) * this->dtsqrt * normalVar;
    return xn + driftTerm + diffusionTerm;
}

template class ModifiedPredictorCorrectorFDM<GBM>;
template class ModifiedPredictorCorrectorFDM<CEV>;