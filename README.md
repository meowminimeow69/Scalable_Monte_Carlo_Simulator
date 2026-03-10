# Scalable Monte Carlo Option Pricer — C++17

A high-performance, policy-based Monte Carlo simulation engine for option pricing, designed with HFT-grade constraints at its core. The system is fully pluggable: any Stochastic Differential Equation (SDE) can be paired with any Finite Difference Method (FDM) and any Random Number Generator (RNG) without modifying the engine internals.

---

## Table of Contents
- [Motivation](#motivation)
- [HFT Design Constraints](#hft-design-constraints)
- [Architecture Overview](#architecture-overview)
- [Design Patterns](#design-patterns)
- [Component Breakdown](#component-breakdown)
- [Parallelism Strategy](#parallelism-strategy)
- [Performance Results](#performance-results)
- [Key C++ Techniques](#key-c-techniques)
- [File Structure](#file-structure)

---

## Motivation

Monte Carlo simulation is the backbone of exotic option pricing — especially for path-dependent products like Asian and Barrier options where closed-form solutions don't exist. The challenge is that convergence requires millions of simulated paths, making raw performance non-negotiable.

This project started as a C# prototype and was redesigned from scratch in C++17 with HFT-grade engineering principles applied throughout — zero heap allocation on the hot path, no virtual dispatch, and full hardware utilization via a lock-free thread pool.

---

## HFT Design Constraints

Every architectural decision in this project is driven by the following constraints, common in latency-sensitive production systems:

| Constraint | Solution Applied |
|---|---|
| No virtual functions / vtables on hot path | CRTP (Compile-Time Polymorphism) |
| No heap allocation on hot path | Pre-allocated flat buffers and path matrices |
| Maximize hardware utilization | Lock-free thread pool with per-thread FDM + RNG instances |
| Pluggable / scalable design | Policy-based templates (SDE, FDM, RNG are independent) |
| Avoid unnecessary copying | `shared_ptr` used throughout for shared ownership |
| Modern design patterns | Function Template Pattern, Mediator, Builder, CRTP |

---

## Architecture Overview

```
User Code
    │
    ▼
MC::Builder  ──── Wires together SDE + FDM + RNG
    │
    ▼
MC::Mediator ──── Hides all complexity; owns the simulation loop
    │
    ├──── Single-Thread Path: Sequential path simulation
    │
    └──── Parallel Path:
              LockFreeThreadPool
                  │
                  ├── Thread 0 → FDM[0] + RNG[0] → FlatBuffer[0..T]
                  ├── Thread 1 → FDM[1] + RNG[1] → FlatBuffer[T+1..2T]
                  ├── Thread 2 → FDM[2] + RNG[2] → FlatBuffer[2T+1..3T]
                  └── ...
                        │
                        ▼
               Mutex-guarded Listener convergence
                        │
                        ▼
                   Finalizer → Priced Output
```

The engine separates **simulation** (Mediator) from **construction** (Builder) and from **payoff computation** (Pricer + Listener functions). This means adding a new SDE, FDM, or option type requires zero changes to the core engine.

---

## Design Patterns

### 1. CRTP — Curiously Recurring Template Pattern
Used across all base classes (`SDEBase`, `FDMBase`, `RNGBase`, `PricerBase`) to achieve **compile-time polymorphism** with zero overhead. The `advance()` function — called 500 million+ times across a 1M-path, 500-step simulation — is fully inlined at compile time. No vtable lookup, no dynamic dispatch, no indirection.

```cpp
// Pattern structure (simplified)
template <typename Derived>
class FDMBase {
public:
    double advance(double t, double dt, double dW) {
        return static_cast<Derived*>(this)->advance_impl(t, dt, dW);
    }
};

class EulerFDM : public FDMBase<EulerFDM> {
public:
    double advance_impl(double t, double dt, double dW) { ... }
};
```

This is the single most impactful optimization — replacing a virtual call with a direct inlined call at a site executed hundreds of millions of times.

### 2. Builder Pattern
The Builder collects the three independently constructed components (SDE, FDM, RNG) and handles the dependency resolution between them. Without it, the caller would need to know that the FDM depends on SDE parameters, that the RNG needs a seed matrix for parallel mode, and that all three must be handed to the Mediator in the correct order. The Builder encapsulates all of this.

### 3. Mediator Pattern
The Mediator owns the simulation loop and acts as the sole point of interaction between the user and the engine internals. It hides the complexity of path generation, thread dispatch, listener notification, and result aggregation. The user calls `StartSimulation()` — the Mediator handles everything else.

### 4. Function Template Pattern (Module 7)
Used for payoff (listener) functions and output (finalizer) functions. Instead of creating full pricer subclasses for every payoff type, the user injects a callable — a lambda or function pointer — directly into the engine. This reduces boilerplate and keeps the design open for extension without modification.

> **Note:** This pattern was deliberately *not* applied to pricer classes. In the hot path, the stack overhead of callable indirection across 500M calls would be measurable. CRTP is used there instead. The Function Template pattern is reserved for finalizers and listeners — called once per simulated path, not once per time step.

---

## Component Breakdown

### SDE (Stochastic Differential Equation)
Defines the dynamics of the underlying asset.
- `GBM.hpp` — Geometric Brownian Motion (standard Black-Scholes dynamics)
- `CEV.hpp` — Constant Elasticity of Variance (volatility smile modeling)

### FDM (Finite Difference Method)
Discretizes the SDE over each time step. 11 schemes implemented, ranging from first-order to high-order methods:
- `EulerFDM` — First-order, simplest
- `MilsteinFDM` / `DiscreteMilsteinFDM` — Second-order correction for diffusion
- `HeunFDM` / `Heun2FDM` — Runge-Kutta-style predictor-corrector
- `MidpointPredictorCorrectorFDM` / `FittedMidpointPredictorCorrectorFDM`
- `PredictorCorrectorFDM` / `ModifiedPredictorCorrectorFDM`
- `FRKIFDM` — Fully implicit Runge-Kutta
- `Platen01ExplicitFDM` — Strong order 1.0 explicit scheme
- `ExactFDM` / `DerivativeFreeFDM`

### RNG (Random Number Generator)
- `BoxMullerRNG` — Standard Box-Muller Gaussian transform
- `PolarMarsagliaRNG` — Rejection-sampling alternative, avoids trig functions
- `ParallelBoxMullerRNG` — Seeded per-thread instance to ensure reproducibility and eliminate contention

### Pricer
Path-dependent payoff computation, all using CRTP:
- `EuropeanPricer` — Max(S_T - K, 0)
- `AsianPricer` — Payoff on average path price
- `BarrierPricer` — Knock-in / knock-out barrier logic

### Mediator & Builder
Orchestration layer. `Builder.hpp` constructs all components. `Mediator.hpp` owns the simulation loop in both single-thread and parallel modes.

### LockFreeThreadPool
Custom thread pool implementation. Uses atomics for task dispatch to eliminate mutex overhead on task submission. Mutex is only used at the convergence point — the listener aggregation step — not during path computation.

---

## Parallelism Strategy

The parallel simulation exploits the fact that **Monte Carlo paths are fully independent** — no path depends on any other.

**Memory layout:**
- A flat buffer of size `Threads × PathSize` is pre-allocated once before simulation starts — no heap allocation during execution
- Each thread owns a contiguous slice: `Thread[i]` operates on `FlatBuffer[i*T .. (i+1)*T]`
- Each thread gets its own `FDM[i]` and `ParallelBoxMullerRNG[i]` instance — zero inter-thread sharing during computation

**Synchronization:**
- Atomic task counter for lock-free work dispatch across the thread pool
- Mutex used only at listener convergence — once per path, not once per time step

**Speedup achieved (Release Mode):**
- Single-thread: 28 seconds
- Multi-thread: 2.6 seconds
- Effective speedup: ~**10.5×** on the test hardware

---

## Performance Results

All results use: S₀ = 100, K = 100, r = 0.05, σ = 0.2, T = 1.0, Steps = 500, Simulations = 1,000,000

### Debug Mode
| Mode | European Call MC | BS Analytical | Diff | Time |
|---|---|---|---|---|
| Single Thread | 10.4458 | 10.4506 | -0.0048 | 203 sec |
| Multi Thread | 10.4526 | 10.4506 | +0.0020 | 12 sec |

### Release Mode
| Mode | European Call MC | BS Analytical | Diff | Time |
|---|---|---|---|---|
| Single Thread | 10.4139 | 10.4506 | -0.0367 | 28 sec |
| Multi Thread | 10.4615 | 10.4506 | +0.0109 | **2.6 sec** |

**Put-Call Parity Verification:**
- C - P (MC): 4.8848
- S - PV(K): 4.8771
- Parity holds ✓

---

## Key C++ Techniques

| Technique | Where Used | Why |
|---|---|---|
| CRTP | All base classes | Compile-time polymorphism, zero virtual overhead |
| `std::shared_ptr` | Component ownership | Shared lifetime across Builder → Mediator |
| `std::atomic` | Thread pool task counter | Lock-free task dispatch |
| `std::mutex` | Listener aggregation | Safe accumulation of path results |
| Lambda functions | Payoff & finalizer injection | Flexible, no-class-needed callables |
| Template parameters | SDE/FDM/RNG wiring | Full type-safety, compiler-enforced correctness |
| Flat buffer pre-allocation | Parallel path matrix | Zero hot-path heap allocation |
| `constexpr` / TMP | Compile-time constants | Avoid runtime cost for fixed parameters |

---

## File Structure

```
├── SDE/
│   ├── SDEBase.hpp
│   ├── GBM.hpp
│   └── CEV.hpp
├── FDM/
│   ├── FDMBase.hpp
│   ├── EulerFDM.hpp
│   ├── MilsteinFDM.hpp
│   ├── DiscreteMilsteinFDM.hpp
│   ├── HeunFDM.hpp
│   ├── Heun2FDM.hpp
│   ├── PredictorCorrectorFDM.hpp
│   ├── MidpointPredictorCorrectorFDM.hpp
│   ├── FittedMidpointPredictorCorrectorFDM.hpp
│   ├── ModifiedPredictorCorrectorFDM.hpp
│   ├── FRKIFDM.hpp
│   ├── Platen01ExplicitFDM.hpp
│   ├── ExactFDM.hpp
│   └── DerivativeFreeFDM.hpp
├── RNG/
│   ├── RNGBase.hpp
│   ├── BoxMullerRNG.hpp
│   ├── PolarMarsagliaRNG.hpp
│   └── ParallelBoxMullerRNG.hpp
├── Pricer/
│   ├── PricerBase.hpp
│   ├── EuropeanPricer.hpp
│   ├── AsianPricer.hpp
│   └── BarrierPricer.hpp
├── Mediator.hpp
├── Builder.hpp
├── LockFreeThreadPool.hpp
└── MonteCarloEngine.cpp
```
