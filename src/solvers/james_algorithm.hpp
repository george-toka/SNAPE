#pragma once

#include "particles.hpp"
#include "uniform_mesh.hpp"
#include "solver_types.hpp"
#include <type_traits>
#include <vector>

template <typename T>
void destroy_workspace(void* ptr) {
    delete static_cast<T*>(ptr);
}

struct JamesSharedData {
    size_t N_bnd;
    std::vector<Real> b_s;
    std::vector<Real> bound_dists;

    JamesSharedData(size_t N, UniformMesh& D) {
        N_bnd = 6 * (N - 2) * (N - 2);
        b_s.resize(N_bnd, 0.0);
        bound_dists.resize(N_bnd * N_bnd, 0.0);

        // Calculate bound_dists
        initialise_bound_dists(N, D);
    }   
    
    void initialise_bound_dists(size_t N, const UniformMesh& D);
};

template <typename Solver>
struct JamesAlgorithm : Solver {
    std::vector<Real> x_zs;
    std::vector<Real> x_zi;
    JamesSharedData* shared_buffers; // Raw pointer to be able to assign after phase 1 initialisation

    static_assert(std::is_base_of_v<PoissonSolver, Solver>, "[COMPILATION ERROR] Chosen solver does not fit with James Algorithm");

    JamesAlgorithm(UniformMesh& D) {
        size_t N = D.N_total;
        x_zs.resize(N*N*N, 0.0);
        x_zi.resize(N*N*N, 0.0);

        if(D.workspaces[WORKSPACE_JAMES] == nullptr) {
            D.workspaces[WORKSPACE_JAMES] = std::unique_ptr<void, SafeDeleter>(
                new JamesSharedData(N, D),
                SafeDeleter{ &destroy_workspace<JamesSharedData> }
            );
        }   

        shared_buffers = static_cast<JamesSharedData*>(D.workspaces[WORKSPACE_JAMES].get());
    }

    void SolverWrapper(UniformMesh& D, std::vector<Real>& x, const std::vector<Real>& b, Real inv_denom, Real C);
};

// Declarations 
void extract_screening_charges(const std::vector<Real>& x_zs, std::vector<Real>& b_s, size_t N_total, Real inv_denom);
void calculate_and_assign_potentials(UniformMesh& D, std::vector<Real>& x, const std::vector<Real>& b_s, const std::vector<Real>& bound_dists, Real C);
void flush_boundaries(std::vector<Real>& x_zs, size_t N_total);

template <typename Solver>
inline void JamesAlgorithm<Solver>::SolverWrapper (UniformMesh& D, std::vector<Real>& x, const std::vector<Real>& b, Real inv_denom, Real C) {
    // We don't flush the interior of zi and zs buffers so that we have a close approximation in subsequent calls and the solver converges faster
    
    // -------------- Solve the zero-state PDE (Homogenous Dirichlet Boundaries) -------------- 
    //solve_poisson(x_zs, b);

    // -------------- Find the boundary values imposed by the screening charges -------------- 

    // Find screening charges
    extract_screening_charges(x_zs, shared_buffers->b_s, D.N_total, inv_denom);

    // Direct summations between the boundaries to define induced boundary values - this one might need generalisation IF possible
    calculate_and_assign_potentials(D, x, shared_buffers->b_s, shared_buffers->bound_dists, C);
    
    // -------------- Solve the zero-state PDE (Homogenous Dirichlet Boundaries) -------------- 
    //solve_poisson(x, b);

    // Combine the two x's to find true x
    for (int i = 0; i < D.N3_total; i++) 
        x[i] = x_zi[i] + x_zs[i];

    // Flush boundaries for the next call
    flush_boundaries(x_zs, D.N_total);
}  