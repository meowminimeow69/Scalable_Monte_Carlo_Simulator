#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <tuple>
#include "fdmbase.hpp"
#include "mediator.hpp"
/*
Builder pattern here buildes the mediator which contains the random engine, FDM and SDE using in our pricing model
Builders creates all the shared pointer and setups its all up 
it is the first step of our process
*/
// Builder will use the template to take in the random RNG
template <typename SDE, typename FDM, typename RNG>
class MCBuilder {
public:
    MCBuilder() = default;
    MCBuilder(const MCBuilder& as) = delete;
    MCBuilder& operator=(const MCBuilder& as) = delete;
    MCBuilder(MCBuilder&& as) = delete;
    MCBuilder& operator=(MCBuilder&& as) = delete;
    ~MCBuilder() = default;


    // We copy the similar method from C# file, like create engine
    static std::shared_ptr<MCMediator<SDE, FDM, RNG>> CreateEngine(
        std::shared_ptr<SDE> sde,
        int steps,
        long simulations) {

        auto rng = std::make_shared<RNG>(); // random engine 

        auto fdm = std::make_shared<FDM>(sde, steps);// sde and fdm is decided
         // mediator is returned which setups up everything and returns the shared pointer 
        return std::make_shared<MCMediator<SDE, FDM, RNG>>(sde, fdm, rng, simulations);
    }
};

#endif