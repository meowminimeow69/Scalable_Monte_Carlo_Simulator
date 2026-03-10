#ifndef PARALLEL_BOX_MULLER_RNG_HPP
#define PARALLEL_BOX_MULLER_RNG_HPP

#include "RNGBase.hpp"
#include <random>
#include <cstdint>
#include <cmath>

// SplitMix64
// I  use this for seed generation for 2^260 seeds unqiue before repeating
// online referenec
class SplitMix64 {
    uint64_t m_state;
public:
    explicit SplitMix64(uint64_t seed) noexcept : m_state(seed) {}

    uint64_t next() noexcept {
        m_state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = m_state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};

// ParallelBoxMullerRNG
// this is pretty much the same as other RNG just that in the input , I take in the seed value as well
class ParallelBoxMullerRNG : public RNGBase<ParallelBoxMullerRNG> {
private:
    std::mt19937_64 m_engine;
    std::uniform_real_distribution<double> m_dist{ 0.0, 1.0 };

    double  m_cachedZ2{ 0.0 };
    bool    m_hasCached{ false };

public:
    explicit ParallelBoxMullerRNG(uint64_t seed) {
        SplitMix64 sm(seed);
        std::seed_seq seq{
            sm.next(), sm.next(), sm.next(), sm.next()
        };
        m_engine.seed(seq);
    }

    ParallelBoxMullerRNG() = delete; // default not allowed

    // Non-copyable as we are using std::vector, we need to prvent this copying from happening
    ParallelBoxMullerRNG(const ParallelBoxMullerRNG&) = delete;
    ParallelBoxMullerRNG& operator=(const ParallelBoxMullerRNG&) = delete;

    // Move-only allowed 
    ParallelBoxMullerRNG(ParallelBoxMullerRNG&&) = default;
    ParallelBoxMullerRNG& operator=(ParallelBoxMullerRNG&&) = default;

    double generateRnImpl() {
      
        if (m_hasCached) {
            m_hasCached = false;
            return m_cachedZ2;
        }

        double u1;
        do { u1 = m_dist(m_engine); } while (u1 < 1e-15);
        const double u2 = m_dist(m_engine);
        const double r = std::sqrt(-2.0 * std::log(u1));
        const double theta = 2.0 * 3.1415159 * u2;
        m_cachedZ2 = r * std::sin(theta);
        m_hasCached = true;
        return r * std::cos(theta);
    }
};

#endif // PARALLEL_BOX_MULLER_RNG_HPP