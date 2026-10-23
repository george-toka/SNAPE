#pragma once

#include "particles.hpp"
#include "no_domain.hpp"
#include "solvers.hpp"
#include "gravity_base.hpp"

// Combination checks
namespace GravityDirect {
    template <typename GravityDomain, typename GravitySolver> 
    struct isValidCombination : std::false_type {};

    template <> struct isValidCombination <NoDomain, NoWrapper> : std::true_type {};
}

struct DirectSum : GravityBase {
    // Fundamental attributes
    const Real e_soft = 0;

    // Temporary buffers for performance gains
    std::vector<Real> inv_smoothed_distance;
    
    static_assert(GravityDirect::isValidCombination<GravityDomain,GravitySolver>::value, 
        "[COMPILATION ERROR] COMBINATION FOR GRAVITY DIRECT SUMMATIONS IS INVALID");

    //DirectSum() = default;

    DirectSum (const InputBlock& Params, GravityDomain& D, GravitySolver& S) : 
        GravityBase(Params, D, S),
        e_soft(getParam<Real>(Params, "DIRECT_SUMS.e_soft"))
    {
        size_t N = getParam<size_t>(Params, "N");
        inv_smoothed_distance.resize((N*(N-1))/2, 0.0);
    }
    
    // Overloaded operator that calls the solving function
    void operator() (Particles& P);

    // Used to (possibly) define timestep
    void getSofteningLength (Real& L) const {
        L = e_soft;
    }

    // Calculates potential energy and hands it to Statistics
    // NEEDS GENERALISATION FOR VARIABLE MASS
    Real compute_potential_energy(Particles& P) {
        Real m = P.m_particle;
        Real potential_energy = 0;

        for (auto inv_r : inv_smoothed_distance)
            potential_energy -= inv_r;
        
        potential_energy *= G * m*m;
        return potential_energy;
    }

};