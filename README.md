SNAPE: Smoothed N-body Astrophysical Particle Engine

SNAPE (Smoothed N-body Astrophysical Particle Engine) is a high-performance, bare-metal C++ framework engineered for large-scale, state-of-the-art astrophysical simulations. Designed from the ground up with advanced High-Performance Computing (HPC) principles, SNAPE bridges complex mathematical modeling with elite software architecture.

Built on a strict philosophy of zero-cost abstraction, the engine leverages and borrows from deep C++ metaprogramming techniques—such as Curiously Recurring Template Patterns (CRTP) / Mixin', template recursion, and static polymorphism—to completely eliminate runtime overhead. At the hardware level, SNAPE is heavily optimized through Struct of Arrays (SoA) memory layouts, branchless programming, and aggressive SIMD alignment to maximize cache locality and execution throughput, further enhanced by rigorous Compile/Link-Time Optimizations (LTO). This robust, unified architectural foundation sets the stage for the ongoing development of dependent, highly complex data structures and a suite of state-of-the-art physics algorithms and computational grid solvers (including advanced Multigrid, Krylov subspace, and Domain Decomposition methods) as well as Lagrangian, Mesh-Free Methods that will drive the engine's simulations.

🚀 Core Software Features

Programmatic Decoupling: SNAPE strictly isolates domains, integrators, physics forces, and mathematical solvers. This allows researchers to seamlessly mix and match components without tangling the underlying logic.

Zero-Cost Abstractions: The engine relies on template metaprogramming and structural binding to route logic. You get the flexibility of an object-oriented design with the execution speed of raw, hardcoded C.

Compile-Time Configuration: Engine capabilities are defined before compilation. This directed compilation strategy guarantees highly optimized, deeply pipelined, and incredibly small binaries tailored specifically for the physics required by your current run, by utilising conditional module inclusion.

State-of-the-Art Modularity: Adding a new physics module or solver does not require rewriting the core loop. The engine is built to be a plug-and-play sandbox for high-performance computing.

Centralized & Decentralized Logic: The architecture supports both globally centralized data structures and decentralized, localized memory slots, allowing the engine to adapt to the specific spatial & temporal requirements of the simulation.

Automated Configuration Management: SNAPE dynamically parses and handles input parameters, automatically generating structured configuration files based on the specific domains and solvers active in the compile-time slots via an integrated Python script.

Fully Pipelined Processes: The main execution loop is a streamlined, pipelined process. Data flows cleanly from domain initialization, through decoupled phase-math operations, and into the integrators with zero overhead.

🗺️ Roadmap & Future Additions

The foundational architectural philosophy of SNAPE—its template routing, compile-time slots, and zero-cost safety nets—is fully established. Moving forward, the engine serves as an expanding framework.

Future updates will continuously introduce new, pluggable modules, including:

Integrators: (e.g., Leapfrog, Runge-Kutta variations, Symplectic integrators).

Mathematical Solvers: (e.g., Multigrid methods, Fast Fourier Transforms (FFT), Barnes-Hut Trees, Fast Multipole Methods).

Physics Frameworks: (e.g., Adaptive Smoothed Particle Hydrodynamics (SPH), Magnetohydrodynamics (MHD)).

Specialized Physical Solvers: Advanced boundary condition handlers, collision detectors, and radiative transfer modules.

🛠️ Getting Started

Prerequisites

A modern C++ compiler supporting C++17 or higher.

CMake (3.10+)

Python (3.8+) - Required for the automated configuration generation scripts.
