#include "parameters.hpp"
#include "solver_types.hpp"

#define W_CYCLE "W"
#define V_CYCLE "V"
#define F_CYCLE "F"

#ifndef SMOOTHER
    #define SMOOTHER "V"
#endif


struct Multigrid : PoissonSolver {
    void FMG();
    void solve_poisson(std::vector<Real>& x, std::vector<Real>& b);
};

