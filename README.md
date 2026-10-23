# SNAPE: Smoothed N-body Astrophysical Particle Engine

**SNAPE** (Smoothed N-body Astrophysical Particle Engine) is a high-performance, bare-metal C++ framework designed for state-of-the-art astrophysical simulations.

Built on a strict philosophy of **zero-cost abstraction**, SNAPE utilizes advanced template metaprogramming and compile-time macros to achieve extreme performance. By completely avoiding runtime polymorphism (`virtual` tables) in its core execution loops, the engine ensures that hardware resources are maximally utilized while keeping the codebase highly modular and extensible.

## 🚀 Core Features

* **Programmatic Decoupling:** SNAPE strictly isolates domains, integrators, physics forces, and mathematical solvers. This allows researchers to seamlessly mix and match components without tangling the underlying logic.
* **Zero-Cost Abstractions:** The engine relies on template metaprogramming and structural binding to route logic. You get the flexibility of an object-oriented design with the execution speed of raw, hardcoded C.
* **Compile-Time Configuration:** Engine capabilities are defined before compilation. This directed compilation strategy guarantees highly optimized, deeply pipelined, and incredibly small binaries tailored specifically for the physics required by your current run.
* **State-of-the-Art Modularity:** Adding a new physics module or solver does not require rewriting the core loop. The engine is built to be a plug-and-play sandbox for high-performance computing.

## 🧠 Architecture Highlights

* **Centralized and Decentralized Domains:** The architecture supports both globally centralized data structures and decentralized, localized memory slots, allowing the engine to adapt to the specific spatial requirements of the simulation.
* **Automated Configuration Management:** SNAPE dynamically parses and handles input parameters, automatically generating structured configuration files based on the specific domains and solvers active in the compile-time slots via an integrated Python script.
* **Fully Pipelined Processes:** The main execution loop is a streamlined, pipelined process. Data flows cleanly from domain initialization, through decoupled phase-math operations, and into the integrators with zero overhead.

## 🗺️ Roadmap & Future Additions

The foundational architectural philosophy of SNAPE—its template routing, compile-time slots, and zero-cost safety nets—is fully established. Moving forward, the engine serves as an expanding framework.

Future updates will continuously introduce new, pluggable modules, including:
* **Integrators:** (e.g., Leapfrog, Runge-Kutta variations, Symplectic integrators).
* **Mathematical Solvers:** (e.g., Multigrid methods, Barnes-Hut Trees, Fast Multipole Methods).
* **Physics Frameworks:** (e.g., Smoothed Particle Hydrodynamics (SPH), Magnetohydrodynamics (MHD)).
* **Specialized Physical Solvers:** Advanced boundary condition handlers, collision detectors, and radiative transfer modules.

## 🛠️ Getting Started

### Prerequisites

* A modern C++ compiler (GCC, Clang, or MSVC) supporting C++17 or higher.
* CMake (3.10+)
* Python (3.8+) - *Required for the automated configuration generation scripts.*
