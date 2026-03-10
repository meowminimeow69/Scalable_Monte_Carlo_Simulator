#ifndef BOX_MULLER_RNG_HPP
#define BOX_MULLER_RNG_HPP

#include"RNGBase.hpp"
/*
This version is not thread safe as it has a internal martrix which is seralized
so if 2 threads acesses it at different times then it messed up with the order
*/
class BoxMullerRNG : public RNGBase<BoxMullerRNG> {
private:
	std::mt19937 engine_;
	std::uniform_real_distribution<double> dist_{ 0.0, 1.0 };
public:
    BoxMullerRNG();

    BoxMullerRNG(const BoxMullerRNG& as) = delete;
    BoxMullerRNG& operator=(const BoxMullerRNG& as) = delete;
    BoxMullerRNG(BoxMullerRNG&& as) = delete;
    BoxMullerRNG& operator=(BoxMullerRNG&& as) = delete;
    ~BoxMullerRNG() = default;

	double generateRnImpl();
};

#endif // !BOX_MULLER_RNG_HPP
