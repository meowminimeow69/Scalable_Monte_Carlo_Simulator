#ifndef CEV_HPP
#define CEV_HPP

#include"SDEBase.hpp"
// this is another SDE method like GBM and it has cpp and hpp files and has the implmention funciton for each
class CEV : public SDEBase<CEV> {
private:
     double mu;   // r
     double vol;  // Constant volatility
     double d;    // Constant dividend yield
     double ic;   // Initial condition
     double exp;  // Expiry
     double b;    // Beta
public:
    CEV(double driftCoefficient, double diffusionCoefficient, double dividendYield, double initialCondition,double expiry, double beta);

    CEV() = delete;
    CEV(const CEV& as) = default;
    CEV& operator=(const CEV& as) = default;
    CEV(CEV&& as) = default;
    CEV& operator=(CEV&& as) = default;
    ~CEV() = default;

    double DriftImpl(double x, double t) const;
    double DiffusionImpl(double x, double t) const;
    double DiffusionDerivativeImpl(double x, double t) const;

    double GetInitialConditionImpl() const;
    void SetInitialConditionImpl(double value);
    double GetExpiryImpl() const;
    void SetExpiryImpl(double value);
};

#endif 
