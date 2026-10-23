#pragma once

#include "parameters.hpp"
#include "particles.hpp"

// Null object to satisfy compiler
struct NoDomain {
    NoDomain() = default;
    NoDomain(const InputBlock&, const Particles&, const std::string&) {}
    void setupMesh(const Particles&) {}
};