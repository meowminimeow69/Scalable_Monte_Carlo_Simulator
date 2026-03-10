#include"MilsteinFDM.hpp"
#include"GBM.hpp"
#include"CEV.hpp"
#include<cmath>

template<typename SDEType>
MilsteinFDM<SDEType>::MilsteinFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions): FDMBase<MilsteinFDM<SDEType>,SDEType>(stochasticEquation, numSubdivisions){}

template<typename SDEType>
double MilsteinFDM<SDEType>::AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2) {
	return xn + this->sde->Drift(xn, tn) * dt + this->sde->Diffusion(xn, tn) * std::sqrt(dt) * normalVar
		+ 0.5 * dt * this->sde->Diffusion(xn, tn) * this->sde->DiffusionDerivative(xn, tn) * (normalVar *normalVar - 1.0);
}

template class MilsteinFDM<GBM>;
template class MilsteinFDM<CEV>;