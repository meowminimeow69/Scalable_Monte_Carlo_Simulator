#include"CEV.hpp"
#include<cmath>

CEV::CEV(double driftCoefficient, double diffusionCoefficient, double dividendYield, double initialCondition, double expiry, double beta) : mu(driftCoefficient) , b(beta),ic(initialCondition), vol(diffusionCoefficient* std::pow(initialCondition, 1.0 - beta)), d(dividendYield), exp(expiry){}

double CEV::DriftImpl(double x, double t) const {
	return (mu - d) * x;
}

double CEV::DiffusionImpl(double x, double t) const {
	return vol * std::pow(x, b);
}

double CEV::DiffusionDerivativeImpl(double x, double t) const {
    if (b > 1.0)
    {
        return vol * b * std::pow(x, b - 1.0);
    }
    else
    {
        return vol * b / std::pow(x, 1.0 - b);
    }
}

double CEV::GetInitialConditionImpl() const {
    return ic; 
}

void CEV::SetInitialConditionImpl(double value) { 
    ic = value; 
}

double CEV::GetExpiryImpl() const {
    return exp;
}

void CEV::SetExpiryImpl(double value) {
    exp = value; 
}