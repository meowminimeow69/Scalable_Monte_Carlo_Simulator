#ifndef SDE_BASE_HPP
#define SDE_BASE_HPP

template<typename Derived>
class SDEBase {
public:

    SDEBase() = default;
    SDEBase(const SDEBase& as) = default;
    SDEBase& operator=(const SDEBase& as) = default;
    SDEBase(SDEBase&& as) = default;
    SDEBase& operator=(SDEBase&& as) = default;
    ~SDEBase() = default;

    double Drift(double x, double t) const {
        return static_cast<const Derived*>(this)->DriftImpl(x, t);
    }

    double Diffusion(double x, double t) const {
        return static_cast<const Derived*>(this)->DiffusionImpl(x, t);
    }

    double DiffusionDerivative(double x, double t) const {
        return static_cast<const Derived*>(this)->DiffusionDerivativeImpl(x, t);
    }

    // this remain same throught all the SDEs
    double DriftCorrected(double x, double t, double B) const {
        return Drift(x, t) - B * Diffusion(x, t) * DiffusionDerivative(x, t);
    }

    // Getters and setters for Intial Condition and Expiry
    double GetInitialCondition() const {
        return static_cast<const Derived*>(this)->GetInitialConditionImpl();
    }
    void SetInitialCondition(double value) {
        static_cast<Derived*>(this)->SetInitialConditionImpl(value);
    }

    double GetExpiry() const {
        return static_cast<const Derived*>(this)->GetExpiryImpl();
    }
    void SetExpiry(double value) {
        static_cast<Derived*>(this)->SetExpiryImpl(value);
    }
};

#endif // !SDE_BASE_HPP
