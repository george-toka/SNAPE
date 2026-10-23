#pragma once
  
#include <limits>
#include "particles.hpp"
#include "domain_manager.hpp"
#include "solver_manager.hpp"
#include "force_defs.hpp"

struct Forces{
    Gravity gravity;
    Hydro hydro;

    Forces(const InputBlock& Params, DomainManager& DM, SolverManager& SM) : 
        gravity(Params, DM.getGravityDomain(), SM.gSolver),
        hydro(Params, DM.getHydroDomain(), SM.hSolver)
    {}

    // Get minimum resolution scale to pass onto timestep criteria
    Real getMinSofteningLength();

    // Acceleration computation gatherer
    void Acceleration(Particles &P);

};
