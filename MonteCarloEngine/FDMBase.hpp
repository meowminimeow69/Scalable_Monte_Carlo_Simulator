#ifndef FDM_BASE_HPP
#define FDM_BASE_HPP

#include<memory>
#include<vector>
#include<cmath>

template<typename Derived,typename SDEType>
class FDMBase {
protected:
	std::shared_ptr<SDEType> sde;
	int NT; // number of time 
	std::vector<double> x; // mesh arry (time points)
	double k; // step size (dt)
	double dtsqrt; 
public:

	FDMBase() = delete;
	FDMBase(const FDMBase& as) = default;
	FDMBase& operator=(const FDMBase& as) = default;
	FDMBase(FDMBase&& as) = default;
	FDMBase& operator=(FDMBase&& as) = default;
	~FDMBase() = default;

	FDMBase(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions) : sde(stochasticEquation), NT(numSubdivisions) {
		k = sde->GetExpiry()/static_cast<double>(NT);
		dtsqrt = std::sqrt(k);
		x.resize(NT + 1); // we resize from before so that we do not have to resize the vector each time , making the process faster

		x[0] = 0.0;
		for (int n = 1; n <= NT; n++) {
			x[n] = x[n - 1] + k;
		}
	}
	// thiss is the path defined advance
	double Advance(double xn, double tn, double dt, double normalVar, double normalVar2)  {
		return static_cast<Derived*>(this)->AdvanceImpl(xn, tn, dt, normalVar, normalVar2);
	}

	// getter setter for sde
	std::shared_ptr<SDEType> GetSde() const { return sde; }
	void SetSde(std::shared_ptr<SDEType> newSde) { sde = newSde; }
	// stepsize for future cal
	double GetStepSize() const { return k; }
	// mesh for average of future analysis
	const std::vector<double>& GetMesh() const { return x; }
};


#endif