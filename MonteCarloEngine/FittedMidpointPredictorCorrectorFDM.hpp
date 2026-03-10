#ifndef FITTED_MIDPOINT_PREDICTOR_CORRECTOR_FDM
#define FITTED_MIDPOINT_PREDICTOR_CORRECTOR_FDM

#include "FDMBase.hpp"
class GBM;
class CEV;
// similar as C#
template <typename SDEType>
class FittedMidpointPredictorCorrectorFDM : public FDMBase<FittedMidpointPredictorCorrectorFDM<SDEType>, SDEType> {
private:
	double A, B, VMid;
public:
	FittedMidpointPredictorCorrectorFDM() = delete;
	FittedMidpointPredictorCorrectorFDM(const FittedMidpointPredictorCorrectorFDM& as) = default;
	FittedMidpointPredictorCorrectorFDM& operator=(const FittedMidpointPredictorCorrectorFDM& as) = default;
	FittedMidpointPredictorCorrectorFDM(FittedMidpointPredictorCorrectorFDM&& as) = default;
	FittedMidpointPredictorCorrectorFDM& operator=(FittedMidpointPredictorCorrectorFDM&& as) = default;
	~FittedMidpointPredictorCorrectorFDM() = default;

	FittedMidpointPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class FittedMidpointPredictorCorrectorFDM<GBM>;
extern template class FittedMidpointPredictorCorrectorFDM<CEV>;

#endif 