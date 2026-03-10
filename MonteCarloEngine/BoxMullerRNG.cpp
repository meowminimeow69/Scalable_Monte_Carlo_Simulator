#include"BoxMullerRNG.hpp"

BoxMullerRNG::BoxMullerRNG() :engine_(std::random_device{}()) {}

double BoxMullerRNG::generateRnImpl() {
    double u1 = dist_(engine_);
    double u2 = dist_(engine_);
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * 3.1415159 * u2);
}