#pragma once

#include "compile_macros.hpp"
#include "force_macros.hpp"
#include "gravity_macros.hpp"
#include "no_framework.hpp"

// Gravity solver selection
#if !GRAVITY_ENABLED 
    #undef GRAVITY_FRAMEWORK
    #define GRAVITY_FRAMEWORK NO_FRAMEWORK
    using Gravity = NoFramework;
#elif GRAVITY_FRAMEWORK == DIRECT_SUMS
    #include "direct_summations_solver.hpp"
    using Gravity = DirectSum;
#elif GRAVITY_FRAMEWORK == GRAVITY_MESH
    #include "gravity_mesh_solver.hpp"
    using Gravity = GravityMeshSolver;
#else
    #error "[COMPILATION ERROR] GRAVITY SOLVER CHOICE IS INVALID"
#endif
