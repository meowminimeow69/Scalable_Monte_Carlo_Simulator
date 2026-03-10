#ifndef RNG_BASE_HPP
#define RNG_BASE_HPP

#include<random>

template <typename Derived>
class RNGBase {
public:

	RNGBase() = default;
	RNGBase(const RNGBase& as) = delete;
	RNGBase& operator=(const RNGBase& as) = delete;
	RNGBase(RNGBase&& as) = default;
	RNGBase& operator=(RNGBase&& as) = default;
	~RNGBase() = default;

    double generateRn() {
        return static_cast<Derived*>(this)->generateRnImpl();
    }
};

#endif 