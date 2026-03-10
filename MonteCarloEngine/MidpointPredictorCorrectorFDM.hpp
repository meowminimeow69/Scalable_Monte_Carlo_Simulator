#ifndef MIDPOINT_PREDICTOR_CORRECTOR_FDM
#define MIDPOINT_PREDICTOR_CORRECTOR_FDM

#include "FDMBase.hpp"
class GBM;
class CEV;

template <typename SDEType>
class MidpointPredictorCorrectorFDM : public FDMBase<MidpointPredictorCorrectorFDM<SDEType>, SDEType> {
private:
	double A, B, VMid;
public:
	MidpointPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b);
    MidpointPredictorCorrectorFDM() = delete;
    MidpointPredictorCorrectorFDM(const MidpointPredictorCorrectorFDM& as) = default;
    MidpointPredictorCorrectorFDM& operator=(const MidpointPredictorCorrectorFDM& as) = default;
    MidpointPredictorCorrectorFDM(MidpointPredictorCorrectorFDM&& as) = default;
    MidpointPredictorCorrectorFDM& operator=(MidpointPredictorCorrectorFDM&& as) = default;
    ~MidpointPredictorCorrectorFDM() = default;
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class MidpointPredictorCorrectorFDM<GBM>;
extern template class MidpointPredictorCorrectorFDM<CEV>;

#endif 
