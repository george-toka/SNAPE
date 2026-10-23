// ==========================================
// 1. MACRO IDs
// ==========================================
#define SOLVER_NONE         0
#define SOLVER_CG           200   
#define SOLVER_MULTIGRID    201   
#define SOLVER_BARNES_HUT   202   

#define WRAPPER_NONE        0
#define WRAPPER_JAMES       300  
#define WRAPPER_TREE        301

// ==========================================
// 2. CONFIGURATION & OVERRIDES
// ==========================================
#ifndef GRAVITY_SOLVER
    #define GRAVITY_SOLVER SOLVER_NONE
#endif
#ifndef GRAVITY_WRAPPER
    #define GRAVITY_WRAPPER WRAPPER_NONE
#endif

#ifndef HYDRO_SOLVER
    #define HYDRO_SOLVER SOLVER_NONE
#endif
#ifndef HYDRO_WRAPPER
    #define HYDRO_WRAPPER WRAPPER_NONE
#endif