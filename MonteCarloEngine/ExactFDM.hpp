#ifndef EXACT_FDM_HPP
#define EXACT_FDM_HPP

#include"FDMBase.hpp"
class GBM;
class CEV;

template<typename SDEType>
class ExactFDM : public FDMBase<ExactFDM<SDEType>, SDEType> {
private:
	double S0;
	double sig;
	double mu;
public:
	ExactFDM() = delete;
	ExactFDM(const ExactFDM& as) = default;
	ExactFDM& operator=(const ExactFDM& as) = default;
	ExactFDM(ExactFDM&& as) = default;
	ExactFDM& operator=(ExactFDM&& as) = default;
	~ExactFDM() = default;

	ExactFDM(std::shared_ptr<SDEType> stochasticEquation, int numSubdivisions, double S0, double vol, double drift);
	double AdvanceImpl(double xn, double tn, double dt, double normalVar, double normalVar2);
};

extern template class ExactFDM<GBM>;
extern template class ExactFDM<CEV>;


#endif 
