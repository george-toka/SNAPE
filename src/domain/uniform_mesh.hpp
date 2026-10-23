#pragma once

#include <memory>
#include "parameters.hpp"
#include "particles.hpp"

enum WorkspaceType {
    WORKSPACE_JAMES = 0,     // James gets spot 0
    WORKSPACE_MULTIGRID = 1, // Multigrid gets spot 1
    MAX_WORKSPACES = 2       // The total number of spots
};


struct SafeDeleter {                                            
    // Stores the lambda function. Defaults to nullptr safely.
    void (*destroy_func)(void*) = nullptr; 

    // The unique_ptr calls this operator when it dies
    void operator()(void* ptr) const {
        // Only execute if both the data AND the destruction manual exist
        if (destroy_func != nullptr && ptr != nullptr) {
            destroy_func(ptr);
        }
    }
};


struct UniformMesh {
    size_t N_mesh; // Number of vertices
    size_t N_pad;  // Number of padding cells PER SIDE
    size_t N_total, N2_total, N3_total;
    Real h, inv_h, inv_h3, L, domain_min[3], domain_max[3], domain_center[3];
    std::vector<Real> mesh_points[3]; // Each vector holds the set of points per axis (CELL CENTERS) 

    // Solver specific data pinpointed to the mesh (useful for cases where we want shared solver data)
    std::unique_ptr<void, SafeDeleter> workspaces[MAX_WORKSPACES];
    //std::unique_ptr<MeshWorkspaceDeleter> workspaces[MAX_WORKSPACES];

    UniformMesh() = default;
    
    UniformMesh(const InputBlock& Params, const Particles& P, const std::string& prefix="") : 
        N_mesh(getParam<size_t>(Params, prefix+"N_mesh")),
        N_pad(getParam<size_t>(Params, prefix+"N_pad")),
        N_total(2*N_pad + N_mesh),
        N2_total(N_total * N_total),
        N3_total(N2_total * N_total)
    {
        for (int i = 0; i < 3; i++)
            mesh_points[i].resize(N_mesh + 2*N_pad);   

        setupMesh(P);
    }

    void setupMesh(const Particles& P);

    // Will be generalised either through overload, templates or sth to use both Real and vector<Real>
    void CIC_P2M(const Particles& P, Real interpolator, std::vector<Real>& interpolant);
    void CIC_M2P(const Particles& P, std::vector<Real>& interpolant);
};