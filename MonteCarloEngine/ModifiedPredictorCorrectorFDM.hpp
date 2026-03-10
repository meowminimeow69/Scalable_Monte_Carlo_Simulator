#ifndef MODIFIED_PREDICTOR_CORRECTOR_FDM
#define MODIFIED_PREDICTOR_CORRECTOR_FDM

#include "FDMBase.hpp"
class GBM;
class CEV;

template <typename SDEType>
class ModifiedPredictorCorrectorFDM : public FDMBase<ModifiedPredictorCorrectorFDM<SDEType>, SDEType> {
private:
	double A, B, VMid;
public:
	ModifiedPredictorCorrectorFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double a, double b);
	
	ModifiedPredictorCorrectorFDM() = delete;
	ModifiedPredictorCorrectorFDM(const ModifiedPredictorCorrectorFDM& as) = default;
	ModifiedPredictorCorrectorFDM& operator=(const ModifiedPredictorCorrectorFDM& as) = default;
	ModifiedPredictorCorrectorFDM(ModifiedPredictorCorrectorFDM&& as) = default;
	ModifiedPredictorCorrectorFDM& operator=(ModifiedPredictorCorrectorFDM&& as) = default;
	~ModifiedPredictorCorrectorFDM() = default;
	
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class ModifiedPredictorCorrectorFDM<GBM>;
extern template class ModifiedPredictorCorrectorFDM<CEV>;

#endif 
