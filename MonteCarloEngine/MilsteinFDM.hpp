#ifndef MILSTEIN_FDM_HPP
#define MILSTEIN_FDM_HPP

#include"FDMBase.hpp"
class GBM;
class CEV;

template<typename SDEType>
class MilsteinFDM : public FDMBase<MilsteinFDM<SDEType>, SDEType> {
public:
	MilsteinFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions);

    MilsteinFDM() = delete;
    MilsteinFDM(const MilsteinFDM& as) = default;
    MilsteinFDM& operator=(const MilsteinFDM& as) = default;
    MilsteinFDM(MilsteinFDM&& as) = default;
    MilsteinFDM& operator=(MilsteinFDM&& as) = default;
    ~MilsteinFDM() = default;

	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class MilsteinFDM<GBM>;
extern template class MilsteinFDM<CEV>;


#endif 
