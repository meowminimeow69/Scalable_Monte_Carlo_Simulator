#include"GBM.hpp"


GBM::GBM(double drift, double diffusion, double dividend, double initial, double expiry) : mu(drift) , vol(diffusion), d(dividend),ic(initial),exp(expiry) {}

double GBM::DriftImpl(double x, double t) const {
	return (mu - d) * x;
}

double GBM::DiffusionImpl(double x, double t) const {
	return vol * x;
}

double GBM::DiffusionDerivativeImpl(double x, double t) const {
	return vol;
}

double GBM::GetInitialConditionImpl() const {
	return ic;
}

void GBM::SetInitialConditionImpl(double val) {
	ic = val;
}

double GBM::GetExpiryImpl() const {
	return exp;
}

void GBM::SetExpiryImpl(double val) {
	exp = val;
}