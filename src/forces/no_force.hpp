#pragma once

#include "particles.hpp"
#include "no_domain.hpp"
#include "no_solver.hpp"

// Null object to satisfy compiler
struct EmptyForce {
    EmptyForce() = default;
    EmptyForce (const InputBlock&, NoDomain&, NoWrapper&) {}
    void operator() (Particles&) {}
    void getSofteningLength(Real&) const {}
};
