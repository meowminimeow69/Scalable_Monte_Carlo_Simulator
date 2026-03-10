#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <vector>
#include <memory>
#include <functional>
#include"LockFreeThreadPool.hpp"
#include "ParallelBoxMullerRNG.hpp"
#include<chrono>

// We template the Mediator with the concrete types of your CRTP components
template <typename SDE, typename FDM, typename RNG>
class MCMediator {
private:
    std::shared_ptr<SDE> m_sde; // sde
    std::shared_ptr<FDM> m_fdm; // fdm
    std::shared_ptr<RNG> m_rng; // random engine

    long m_numSimulations;

    // I have included policy based function design pattern which is used in the module 7 
    std::vector<std::function<void(const std::vector<double>&)>> m_pathListeners;
    std::vector<std::function<void()>> m_finalizers; // this is for the final print out

    std::unique_ptr<LockFreeThreadPool> m_threadPool;// our thread pool 

public:
    MCMediator(std::shared_ptr<SDE> sde, std::shared_ptr<FDM> fdm, std::shared_ptr<RNG> rng, long sims)
        : m_sde(sde), m_fdm(fdm), m_rng(rng), m_numSimulations(sims) {
    }

    MCMediator() = delete;
    MCMediator(const MCMediator& as) = default;
    MCMediator& operator=(const MCMediator& as) = default;
    MCMediator(MCMediator&& as) = default;
    MCMediator& operator=(MCMediator&& as) = default;
    ~MCMediator() = default;

    // I addined this for 2 ways so that I can have processer(ie the payout curve) and finalizer which is the printout in our case
    void Subscribe(std::function<void(const std::vector<double>&)> pathFunc, std::function<void()> endFunc) {
        m_pathListeners.push_back(pathFunc);
        m_finalizers.push_back(endFunc);
    }
    // using the C# logic here
    void StartSimulation() {
        int NT = m_fdm->GetMesh().size() - 1;
        double dt = m_fdm->GetStepSize();
        std::vector<double> path(NT + 1);
      

        for (long i = 0; i < m_numSimulations; ++i) {
            // Initial condition all our condition is 0.0
            path[0] = m_sde->GetInitialCondition();

            // Path Generation (Inner Loop)
            for (int n = 0; n < NT; ++n) {
                double dw = m_rng->generateRn();
                double dw2 = m_rng->generateRn(); //this never gets used in any of our FDE but still
                path[n + 1] = m_fdm->Advance(path[n], n * dt, dt, dw, dw2);
            }

            /*
            I did not use boost signal2 here for a reason  as I wanted to expand this to multiple threads
            it is basically when I divide the into multiple simulation, I cannot do this directly use boost signal

            here in our case the lister is basically like a summuation function so we cannot use boost signal when the simulation is broken completely

            I will explain in detial in my parallel simulation code
            */
            for (auto& list : m_pathListeners) {
                list(path);
            }
        }

        // Notify pricers to finish and I also consiedr adding the discount factor here and iostream function
        for (auto& finalizer : m_finalizers) {
            finalizer();
        }
    }
    // the above code is exteremely sequential making it exteremly slow 

    /*
    On closer observation , each simulation path is independent of each other so I can break total_number of simulation
    into based on the number of threads existing
    */

    void StartSimulationParallel(
        int numThreads = static_cast<int>(std::thread::hardware_concurrency()))
    {
        // I make the threadpool
        if (!m_threadPool || m_threadPool->workerCount() != numThreads)
            m_threadPool = std::make_unique<LockFreeThreadPool>(numThreads);

        int NT = m_fdm->GetMesh().size() - 1;
        double dt = m_fdm->GetStepSize();
        int sliceLen = NT + 1; // max len size


        /*
            I was gettng alot of errors here when seeing the BlackSholes computed value vs MonteCarlo values
            I was getting a massive 0.7 difference so turns out it was due to the variables inside the FDM class 
            they were getting over written and becomming thread unsafe, so inorder to make it thread safe, I will be using reference unqiue own the 
            usage below
        */
        std::vector<std::shared_ptr<FDM>> fdms;
        fdms.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
            fdms.push_back(std::make_shared<FDM>(m_sde, NT));

       // this is the buffer for a given path , I will do the path writing in this 
        std::vector<double> flatBuffer(
            static_cast<std::size_t>(numThreads) * sliceLen, 0.0);// it is vector of size of threads*slice lenght

        // basically the use of this vector is that I can allow the thread to work in a specfic bound like 
        // thread1 in 0 to NT , thread 2 - NT+1, 2NT ...... 
            
        // this is like a matirx of (number_of_threads * path_len) ( this way I will reuse the paths without having to recreating it in the hot path)
        // so once the thread process the path, then It will reuse for another thread if needed
        std::vector<std::vector<double>> threadPathMatrix(
            numThreads, std::vector<double>(sliceLen));

        /*
            Issue with simultion with seed 42 and seed 43 is that the numbers get repeated and this causes issues with the MC simulation not being indepedent
            so inorder to prevent that from happening, I will use a SplitMix64 method, I read online where in I will 

            choose a bunch of randomly generated seeds (vector of them) and each of the thread will choose one of the seed and then simulation it

            This way our Random engine becomes safe
        */
        const uint64_t baseSeed = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now()
            .time_since_epoch().count());

        SplitMix64 seeder(baseSeed);// genereates the matrix
        std::vector<ParallelBoxMullerRNG> rngs; // I have only allowed for 1 RNG for parrallel
        rngs.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
            rngs.emplace_back(seeder.next());  /// saves all the random sseed

        // adding all the batch size per thread
        const long batchSize =
            (m_numSimulations + static_cast<long>(numThreads) - 1)
            / static_cast<long>(numThreads);

        // since we are using the function method, I only want 1 thread to access it at a given time
        // as we have our call_sum  and put_sum in our main fucntion and it can lead to race condition
        // inorder to avoid that I have considered this
        std::mutex listenerMutex;

        const double S0 = m_sde->GetInitialCondition(); // remains same for all our cases
        
        // our thread pool takes in our lambha funciton to run
        for (int t = 0; t < numThreads; ++t) {
            m_threadPool->submit([&, t]() {

                // When a thread is create , its stack is made unique, so thread variables are not shared
                double* curr_slice = flatBuffer.data() + t * sliceLen; // this points to the starting of pointer in the buffer zone
                /*
                This is a pointer in the flatbuffer vector where we point to a specfic section it can work on , so this way we do not create any hot paths in the process
                */
                std::vector<double>& myPathVec = threadPathMatrix[t];// we will overide this matrix row ( this is so that lister and the main array does not get current path matrix do not get effected)
                ParallelBoxMullerRNG& myRng = rngs[t]; // unique Random engine with its own seed
                FDM& myFdm = *fdms[t];// unique FDM to prevent sharing of the variables in FDM class

                 long simStart = static_cast<long>(t) * batchSize; 
                 long simEnd = std::min(simStart + batchSize, m_numSimulations); // end condition

                for (long i = simStart; i < simEnd; ++i) {

                    // Usual path generation in that flat buffer vector (myslice pointer)
                    curr_slice[0] = S0;
                    for (int n = 0; n < NT; ++n) {
                        double dw = myRng.generateRn();
                        double dw2 = myRng.generateRn(); 
                        curr_slice[n + 1] =
                            myFdm.Advance(curr_slice[n], n * dt, dt, dw, dw2);
                    }

                    // we copy from the splice pointer 
                    std::copy(curr_slice, curr_slice + sliceLen, myPathVec.begin());

                    /*
                    Our lister functions need a std::vector to evaluate the payout etc, now 
                    I also have flatbuffer worked upon by multiple threads, so 
                    I need to create a layer of seperation between rows which are being worked upon and 
                    rows which are done and ready to be process  so this is why I have the threadpathmatrix
                    here all the processed paths are present and using mutex only 1 thread can access it , this way 
                    I prevent the current slice from being written on while its being listen to so I have the threadpathmatrix be in a mutex
                    so that only ones the flatbuffer path is ready, it can be copied and then processed ahead
                    */
                    {
                        std::lock_guard<std::mutex> lock(listenerMutex);
                        for (auto& listener : m_pathListeners)
                            listener(myPathVec);
                    }
                }
                });
        }

        // I wait for all threads to finish 
         m_threadPool->waitAll();

         // finalizer is the printout path
         for (auto& finalizer : m_finalizers)
            finalizer();
    }
};

#endif