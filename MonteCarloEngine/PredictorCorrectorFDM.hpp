#ifndef PREDICTOR_CORRECTOR_FDM
#define PREDICTOR_CORRECTOR_FDM

#include "FDMBase.hpp"
class GBM;
class CEV;

template <typename SDEType>
class PredictorCorrectorFDM : public FDMBase<PredictorCorrectorFDM<SDEType>, SDEType> {
private:
	double A, B, VMid;
public:
	PredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions,double a,double b);
	
	PredictorCorrectorFDM() = delete;
	PredictorCorrectorFDM(const PredictorCorrectorFDM& as) = default;
	PredictorCorrectorFDM& operator=(const PredictorCorrectorFDM& as) = default;
	PredictorCorrectorFDM(PredictorCorrectorFDM&& as) = default;
	PredictorCorrectorFDM& operator=(PredictorCorrectorFDM&& as) = default;
	~PredictorCorrectorFDM() = default;
	
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class PredictorCorrectorFDM<GBM>;
extern template class PredictorCorrectorFDM<CEV>;

#endif 
