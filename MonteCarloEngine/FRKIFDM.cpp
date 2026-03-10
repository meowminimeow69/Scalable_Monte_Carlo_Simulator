#include "FRKIFDM.hpp"
#include "Gbm.hpp"
#include "Cev.hpp"
#include<cmath>

template<typename SDEType>
FRKIFDM<SDEType>::FRKIFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<FRKIFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {}

template <typename SDEType>
double FRKIFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double dt1 = dt;
    sqrk = std::sqrt(dt1);
    Wincr = sqrk * normalVar;
    F1 = this->sde->Drift(xn, tn);
    G1 = this->sde->Diffusion(xn, tn);
    G2 = this->sde->Diffusion(xn + 0.5 * G1 * (Wincr - sqrk), tn);
    return xn + (F1 * dt1 + G2 * Wincr + (G2 - G1) * sqrk);
}

template class FRKIFDM<GBM>;
template class FRKIFDM<CEV>;