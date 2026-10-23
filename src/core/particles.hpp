#pragma once

#include <iostream>
#include "parameters.hpp"
#include "io_utils.hpp"
#include <vector>

// #####################################
//       PARTICLE DATA CONTAINER
// #####################################

struct Particles {
    // Basic attributes
    size_t N;
    Real M, m_particle; // Mass should be a struct nested in here with its own inline getter function and compile-time configuration

    // SoA to hold particles' information
    std::vector<Real> positions[3];
    std::vector<Real> velocities[3];
    std::vector<Real> accelerations[3];

    // Temporary buffers for performance gains
    std::vector<Real> v2;

    // Cconstructor
    Particles(const InputBlock& Params) {
        // Extract the parameters that this struct needs and perform necessary assignments
        std::tie(this->M, this->N) = unpackParams<Real, size_t>(Params, "M", "N");
        m_particle = M / static_cast<Real>(N);
    }

    // Allocate only state vector - Used for IC Generation phase
    void allocate_initial()
    {
        for (int d = 0; d < 3; d++) {
            positions[d].resize(this->N);
            velocities[d].resize(this->N);
        }
    }

    // Allocate all vectors - Used for Simulation phase
    void allocate_state()
    {
        for (int d = 0; d < 3; d++) {
            positions[d].resize(this->N, 0.0);
            velocities[d].resize(this->N, 0.0);
            accelerations[d].resize(this->N, 0.0);
        }

        v2.resize(this->N, 0.0);
    }

    void flushData() {
        for (auto& acceleration : accelerations)
            std::fill(acceleration.begin(), acceleration.end(), 0);
    }
};
