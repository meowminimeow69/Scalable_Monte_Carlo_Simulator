#include"DiscreteMilsteinFDM.hpp"
#include"GBM.hpp"
#include"CEV.hpp"
#include<cmath>

template<typename SDEType>
DiscreteMilsteinFDM<SDEType>::DiscreteMilsteinFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : FDMBase<DiscreteMilsteinFDM<SDEType>, SDEType>(stochasticEquation, numSubdivisions) {}

template<typename SDEType>
double DiscreteMilsteinFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
    double dt1 = dt; 
    double sqrt = std::sqrt(dt1);
    double a = this->sde->Drift(xn, tn);
    double b = this->sde->Diffusion(xn, tn);
    double Yn = xn + a * dt1 + b * sqrt;
    return xn + a * dt1 + b * sqrt * normalVar + 0.5 * sqrt * (this->sde->Diffusion(Yn, tn) - b) * (normalVar * normalVar - 1.0);
}

template class DiscreteMilsteinFDM<GBM>;
template class DiscreteMilsteinFDM<CEV>;