#include"ExactFDM.hpp"
#include"GBM.hpp"
#include"CEV.hpp"
#include<cmath>

template<typename SDEType>
ExactFDM<SDEType>::ExactFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double S0, double vol, double drift) : FDMBase<ExactFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {
    this->S0 = S0;
    sig = vol;
    mu = drift;
}

template<typename SDEType>
double ExactFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double alpha = 0.5 * sig * sig;
    return S0 * std::exp((mu - alpha) * (tn + dt) + sig * std::sqrt(tn + dt) * normalVar);
}


template class ExactFDM<GBM>;
template class ExactFDM<CEV>;