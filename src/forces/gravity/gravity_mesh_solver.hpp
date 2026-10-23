#pragma once

#include "parameters.hpp"
#include "particles.hpp"
#include "domain.hpp"
#include "gravity_base.hpp"
#include "solvers.hpp"

// Define valid combinations for GravityMeshSolver
namespace GravityMeshCombos {
    template <typename DomainType, typename Solver>
    struct isValidCombination {static constexpr bool value = false;};

    template <typename Solver> struct isValidCombination<UniformMesh, JamesAlgorithm<Solver>> {static constexpr bool value = true;};
}

struct GravityMeshSolver : GravityBase {
    std::vector<Real> rho;
    std::vector<Real> phi;
    std::vector<Real> g[3];

    static_assert(GravityMeshCombos::isValidCombination<GravityDomain,GravitySolver>::value, 
        "[COMPILATION ERROR] COMBINATION OF SOLVER AND DOMAIN TYPE IS INVALID FOR GRAVITY MESH SOLVER");

    //GravityMeshSolver() = default;
    
    GravityMeshSolver(const InputBlock& Params, GravityDomain& D, GravitySolver& S) : 
        GravityBase(Params, D, S)
    {
        size_t N = domain.N3_total;

        rho.resize(N, 0.0);
        phi.resize(N, 0.0);

        for (int i = 0; i < 3; i++) 
            g[i].resize(N, 0.0);
    }

    void operator() (Particles& P);

    void getSofteningLength(Real& L) const {
        L = domain.h;
    }

    Real compute_potential_energy(Particles&) {return 0;}
};