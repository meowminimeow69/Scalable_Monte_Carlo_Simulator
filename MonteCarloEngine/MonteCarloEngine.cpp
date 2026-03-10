#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>
#include "Gbm.hpp"
#include "Cev.hpp"
#include"EulerFDM.hpp"
#include"ExactFDM.hpp"
#include"MilsteinFDM.hpp"
#include"DiscreteMilsteinFDM.hpp"
#include"PredictorCorrectorFDM.hpp"
#include"ModifiedPredictorCorrectorFDM.hpp"
#include"MidpointPredictorCorrectorFDM.hpp"
#include"FittedMidpointPredictorCorrectorFDM.hpp"
#include "Platen01ExplicitFDM.hpp"
#include"HeunFDM.hpp"
#include "DerivativeFreeFDM.hpp"
#include"FRKIFDM.hpp"
#include "Heun2FDM.hpp"


#include"BoxMullerRNG.hpp"
#include"PolarMarsagliaRNG.hpp"

#include"EuropeanPricer.hpp"
#include"BarrierPricer.hpp"

#include"RNGBase.hpp"
#include"Builder.hpp"
#include"Mediator.hpp"


#include<chrono>
// I am using the solution of black solves model
 double N(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
 double BSCall(double S, double K, double r, double q, double sigma, double T) {
    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S * std::exp(-q * T) * N(d1) - K * std::exp(-r * T) * N(d2);
}
 double BSPut(double S, double K, double r, double q, double sigma, double T) {
    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return K * std::exp(-r * T) * N(-d2) - S * std::exp(-q * T) * N(-d1);
}


int main() {
    /*
    I am only comparing european options in the main file as I can solve it effectively
    so I am comparing the call put parity with MC vs Black scholes methods to show the effectiveness of my methods

    I also have a parrallel process and a single pricess
    Parallel process is 16x faster(my machine is 16 cores) faster
    
    */
    std::cout << std::fixed << std::setprecision(10);
    const double S0 = 100.0;   // Spot price
    const double K = 100.0;   // Strike
    const double r = 0.05;    // Risk-free rate
    const double sigma = 0.20;    // Volatility
    const double T = 1.0;     // Expiry (years)
    const double q = 0.0;     // Dividend yield
    const int    NT = 500;     // Time steps (daily)
    const long   NSim = 1000000; // Number of Monte Carlo paths

    // type easure  as we are using this in the pricers
    using PayoffFunc = std::function<double(double)>;
    using DiscountFunc = std::function<double()>;

    {
     
        std::cout << "Monte Carlo European Option Pricer(Single Thread) " << std::endl<<std::endl;
        std::cout << "Parameters: S0=" << S0 << "  K=" << K
            << "  r =" << r << "  sigma =" << sigma
            << "  T =" << T << "  steps =" << NT
            << "  sims =" << NSim << std::endl << std::endl;

     
        auto gbm = std::make_shared<GBM>(r, sigma, q, S0, T);
       

        PayoffFunc   callPayoff = [K](double S) { return std::max(S - K, 0.0); };
        PayoffFunc   putPayoff = [K](double S) { return std::max(K - S, 0.0); };
        DiscountFunc discounter = [r, T]() { return std::exp(-r * T);     };

        EuropeanPricer<PayoffFunc, DiscountFunc> callPricer(callPayoff, discounter);
        EuropeanPricer<PayoffFunc, DiscountFunc> putPricer(putPayoff, discounter);

        // this is the global variables for sum , this is for the fucntion based desing pattern
        double callSum = 0.0;
        double putSum = 0.0;

        auto mediator = MCBuilder<GBM, EulerFDM<GBM>, BoxMullerRNG>::CreateEngine(gbm, NT, NSim);

        // we add the subscribers and finalized in the other we want
        // from a given path , we can compute both the call and put option price so I am doing the same
        mediator->Subscribe(
            [&](const std::vector<double>& path) {
                callSum += callPricer.ProcessPath(path);
            },
            [&]() {
                double price = callPricer.FinalPrice(callSum, NSim);
                double bs = BSCall(S0, K, r, q, sigma, T);
                std::cout << "European Call  ->  MC: " << price
                    << "   BS: " << bs
                    << "   diff: " << (price - bs) << std::endl;
            }
        );
        // for put
        mediator->Subscribe(
            [&](const std::vector<double>& path) {
                putSum += putPricer.ProcessPath(path);
            },
             [&]() {
                double price = putPricer.FinalPrice(putSum, NSim);
                double bs = BSPut(S0, K, r, q, sigma, T);
                std::cout << "European Put   ->  MC: " << price
                    << "   BS: " << bs
                    << "   diff: " << (price - bs) << std::endl;
            }
        );

      
        std::cout << "Running simulation..."<<std::endl<<std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        mediator->StartSimulation();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::endl;
        std::cout << "Time taken for simulation : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microsecond and ie is " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " second " << std::endl;

      
        double callMC = callPricer.FinalPrice(callSum, NSim);
        double putMC = putPricer.FinalPrice(putSum, NSim);
        double parity = callMC - putMC;
        double parityBS = S0 * std::exp(-q * T) - K * std::exp(-r * T);
        std::cout << std::endl;
        std::cout << "Put-Call Parity Check:"<<std::endl;
        std::cout << "  C - P (MC): " << parity << std::endl;
        std::cout << "  S - PV(K) : " << parityBS << std::endl;
    }
    std::cout << std::endl;
    {
        {

            std::cout << "Monte Carlo European Option Pricer(Single Thread) " << std::endl << std::endl;
            std::cout << "Parameters: S0=" << S0 << "  K=" << K
                << "  r =" << r << "  sigma =" << sigma
                << "  T =" << T << "  steps =" << NT
                << "  sims =" << NSim << std::endl << std::endl;



            auto gbm = std::make_shared<GBM>(r, sigma, q, S0, T);


            PayoffFunc   callPayoff = [K](double S) { return std::max(S - K, 0.0); };
            PayoffFunc   putPayoff = [K](double S) { return std::max(K - S, 0.0); };
            DiscountFunc discounter = [r, T]() { return std::exp(-r * T);     };

            EuropeanPricer<PayoffFunc, DiscountFunc> callPricer(callPayoff, discounter);
            EuropeanPricer<PayoffFunc, DiscountFunc> putPricer(putPayoff, discounter);

            // this is the global variables for sum , this is for the fucntion based desing pattern
            double callSum = 0.0;
            double putSum = 0.0;



            auto mediator = MCBuilder<GBM, EulerFDM<GBM>, BoxMullerRNG>::CreateEngine(gbm, NT, NSim);

            // we add the subscribers and finalized in the other we want
            // from a given path , we can compute both the call and put option price so I am doing the same
            mediator->Subscribe(
                [&](const std::vector<double>& path) {
                    callSum += callPricer.ProcessPath(path);
                },
                [&]() {
                    double price = callPricer.FinalPrice(callSum, NSim);
                    double bs = BSCall(S0, K, r, q, sigma, T);
                    std::cout << "European Call  ->  MC: " << price
                        << "   BS: " << bs
                        << "   diff: " << (price - bs) << std::endl;
                }
            );
            // for put
            mediator->Subscribe(
                [&](const std::vector<double>& path) {
                    putSum += putPricer.ProcessPath(path);
                },
                [&]() {
                    double price = putPricer.FinalPrice(putSum, NSim);
                    double bs = BSPut(S0, K, r, q, sigma, T);
                    std::cout << "European Put   ->  MC: " << price
                        << "   BS: " << bs
                        << "   diff: " << (price - bs) << std::endl;
                }
            );


            std::cout << "Running simulation parallel ..." << std::endl << std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            mediator->StartSimulationParallel();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::endl;
            std::cout << "Time taken for parallel simulation : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microsecond and ie is " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " second " << std::endl;


            double callMC = callPricer.FinalPrice(callSum, NSim);
            double putMC = putPricer.FinalPrice(putSum, NSim);
            double parity = callMC - putMC;
            double parityBS = S0 * std::exp(-q * T) - K * std::exp(-r * T);
            std::cout << std::endl;
            std::cout << "Put-Call Parity Check:" << std::endl;
            std::cout << "  C - P (MC): " << parity << std::endl;
            std::cout << "  S - PV(K) : " << parityBS << std::endl;
        }

    }

    
    return 0;
}