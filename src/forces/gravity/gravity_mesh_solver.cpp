#include "compile_macros.hpp"
#include "gravity_macros.hpp"

#if GRAVITY_FRAMEWORK == GRAVITY_MESH

#include "particles.hpp"
#include "gravity_mesh_solver.hpp"

void GravityMeshSolver::operator() (Particles& P) {
    // Solve the poisson equation for a uniform mesh. Flexibility will be added later
}

#endif