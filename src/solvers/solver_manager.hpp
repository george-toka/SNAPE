#pragma once

#include "solvers.hpp"
#include "domain_manager.hpp"

struct SolverManager {
    GravitySolver gSolver;
    HydroSolver hSolver;

    SolverManager(DomainManager& DM) :
        gSolver(DM.getGravityDomain()),
        hSolver(DM.getHydroDomain())
    {}
};