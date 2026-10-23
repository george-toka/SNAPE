#pragma once

#include "parameters.hpp"
#include "domain.hpp"
#include "solvers.hpp"

// Head struct of gravity
struct GravityBase {
    const Real G = 0;
    GravityDomain& domain;
    GravitySolver& solver;

    GravityBase(const InputBlock& Params, GravityDomain&D, GravitySolver& S) :
        G(getParam<Real>(Params, "G")),
        domain(D),
        solver(S)
    {}
};