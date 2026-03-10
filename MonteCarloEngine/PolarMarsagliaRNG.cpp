#include"PolarMarsagliaRNG.hpp"

PolarMarsagliaRNG::PolarMarsagliaRNG() :engine_(std::random_device{}()) {}

double PolarMarsagliaRNG::generateRnImpl() {
    double u, v, S;
    do {
        u = 2.0 * dist_(engine_) - 1.0;
        v = 2.0 * dist_(engine_) - 1.0;
        S = u * u + v * v;
    } while (S >= 1.0);
    return u * std::sqrt(-2.0 * std::log(S) / S);
}