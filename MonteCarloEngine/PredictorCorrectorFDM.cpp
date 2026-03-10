#include"PredictorCorrectorFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"

template<typename SDEType>
PredictorCorrectorFDM<SDEType>::PredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b) : FDMBase<PredictorCorrectorFDM<SDEType>,SDEType>(stochasticEquation, numSubdivisions), A(a), B(b), VMid(0.0){}

template <typename SDEType>
double PredictorCorrectorFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    VMid = xn + this->sde->Drift(xn, tn) * dt + this->sde->Diffusion(xn, tn) * this->dtsqrt * normalVar;

    double driftdoubleTerm = (A * this->sde->Drift(VMid, tn + dt) + ((1.0 - A) * this->sde->Drift(xn, tn))) * dt;
    double diffusiondoubleTerm = (B * this->sde->Diffusion(VMid, tn + dt) + ((1.0 - B) * this->sde->Diffusion(xn, tn))) * this->dtsqrt * normalVar;
    return xn + driftdoubleTerm + diffusiondoubleTerm;
}

template class PredictorCorrectorFDM<GBM>;
template class PredictorCorrectorFDM<CEV>;