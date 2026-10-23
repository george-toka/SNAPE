#pragma once

#include <type_traits>
#include "compile_macros.hpp"
#include "force_macros.hpp"
#include "solver_macros.hpp"
#include "particles.hpp"
#include "no_solver.hpp"

// ==========================================
// CONDITIONAL INCLUDES
// Only parse the headers that are actively requested by at least one force.
// ==========================================
// Undefine and route to NONE if the force module is globally disabled
#if !GRAVITY_ENABLED
    #undef GRAVITY_SOLVER
    #define GRAVITY_SOLVER SOLVER_NONE
    #undef GRAVITY_WRAPPER
    #define GRAVITY_WRAPPER WRAPPER_NONE
#endif

#if !HYDRO_ENABLED
    #undef HYDRO_SOLVER
    #define HYDRO_SOLVER SOLVER_NONE
    #undef HYDRO_WRAPPER
    #define HYDRO_WRAPPER WRAPPER_NONE
#endif

template<int SolverID> struct CoreSelector : std::false_type {};
template<> struct CoreSelector<SOLVER_NONE>       : std::true_type { using type = NoSolver; };

template<int WrapperID> struct WrapperSelector : std::false_type {};
template<> struct WrapperSelector<WRAPPER_NONE>  : std::true_type { 
    template <typename S> using type = NoWrapper; 
};

#if GRAVITY_SOLVER == SOLVER_CG || HYDRO_SOLVER == SOLVER_CG
    #include "cg_solver.hpp"
#endif

#if GRAVITY_SOLVER == SOLVER_MULTIGRID || HYDRO_SOLVER == SOLVER_MULTIGRID
    #include "multigrid.hpp"
    template<> struct CoreSelector<SOLVER_MULTIGRID>  : std::true_type { using type = Multigrid; };
#endif

#if GRAVITY_SOLVER == SOLVER_BARNES_HUT || HYDRO_SOLVER == SOLVER_BARNES_HUT
    #include "barnes_hut_core.hpp"
    template<> struct WrapperSelector<WRAPPER_TREE>  : std::true_type { 
    template <typename S> using type = TreeAlgorithm<S>; 
    };
#endif

#if GRAVITY_WRAPPER == WRAPPER_JAMES || HYDRO_WRAPPER == WRAPPER_JAMES
    #include "james_algorithm.hpp"
    template<> struct WrapperSelector<WRAPPER_JAMES> : std::true_type { 
    template <typename S> using type = JamesAlgorithm<S>; 
    };
#endif

#if GRAVITY_WRAPPER == WRAPPER_TREE || HYDRO_WRAPPER == WRAPPER_TREE
    #include "tree_algorithm.hpp"
#endif

// ==========================================
// 4. STRICT VALIDATION 
// ==========================================
constexpr bool isValidPairing(int wrapper, int solver) {
    if (wrapper == WRAPPER_NONE)  return true; 
    if (wrapper == WRAPPER_JAMES) return (solver == SOLVER_CG || solver == SOLVER_MULTIGRID);
    if (wrapper == WRAPPER_TREE)  return (solver == SOLVER_BARNES_HUT);
    return false; 
}

// Gravity Validation
static_assert(CoreSelector<GRAVITY_SOLVER>::value, "[COMPILATION ERROR] Invalid Gravity Core Solver ID!");
static_assert(WrapperSelector<GRAVITY_WRAPPER>::value, "[COMPILATION ERROR] Invalid Gravity Wrapper ID!");
static_assert(isValidPairing(GRAVITY_WRAPPER, GRAVITY_SOLVER), "[COMPILATION ERROR] Incompatible Wrapper and Core Solver paired for Gravity!");

// Hydro Validation
static_assert(CoreSelector<HYDRO_SOLVER>::value, "[COMPILATION ERROR] Invalid Hydro Core Solver ID!");
static_assert(WrapperSelector<HYDRO_WRAPPER>::value, "[COMPILATION ERROR] Invalid Hydro Wrapper ID!");
static_assert(isValidPairing(HYDRO_WRAPPER, HYDRO_SOLVER), "[COMPILATION ERROR] Incompatible Wrapper and Core Solver paired for Hydro!");

// ==========================================
// 5. FINAL TYPE RESOLUTION 
// ==========================================
using GravityCoreType = typename CoreSelector<GRAVITY_SOLVER>::type;
using GravitySolver   = typename WrapperSelector<GRAVITY_WRAPPER>::template type<GravityCoreType>;

using HydroCoreType   = typename CoreSelector<HYDRO_SOLVER>::type;
using HydroSolver     = typename WrapperSelector<HYDRO_WRAPPER>::template type<HydroCoreType>;