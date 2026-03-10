#ifndef GBM_HPP
#define GBM_HPP

#include"SDEBase.hpp"
// I am using CRTP
class GBM : public SDEBase<GBM> {
private:
     double mu;   // Drift
     double vol;  // Constant volatility
     double d;    // Constant dividend yield
     double ic;   // Initial condition
     double exp;  // Expiry
public:
    GBM(double drift, double diffusion, double dividend, double initial, double expiry);
    GBM() = delete;
    GBM(const GBM& as) = default;
    GBM& operator=(const GBM& as) = default;
    GBM(GBM&& as) = default;
    GBM& operator=(GBM&& as) = default;
    ~GBM() = default;


    double DriftImpl(double x, double t) const;
    double DiffusionImpl(double x, double t) const;
    double DiffusionDerivativeImpl(double x, double t) const;

    // getter and setters
    double GetInitialConditionImpl() const;
    void SetInitialConditionImpl(double value);
    double GetExpiryImpl() const;
    void SetExpiryImpl(double value);
};

#endif 
