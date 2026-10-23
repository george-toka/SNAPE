#pragma once

#include <concepts>
#include "compile_macros.hpp"
#include "integrator_macros.hpp"
#include "integrators.hpp"

// Compile-Time Selection 
#if INTEGRATOR == RK_2
    using Integrator = RK2;
#elif INTEGRATOR == VELOCITY_VERLET
    using Integrator = VelocityVerlet;
#else
    #error "[COMPILATION ERROR] INTEGRATOR CHOICE IS INVALID"
#endif
