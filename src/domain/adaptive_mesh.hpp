#pragma once

#include "particles.hpp"
#include "parameters.hpp"

struct AdaptiveMesh {

    AdaptiveMesh() = default;

    AdaptiveMesh(const InputBlock& Params, const Particles& P, const std::string& prefix="") {}

    void setupMesh(const Particles& P);
};