#pragma once

#include <algorithm>
#include "particles.hpp"
#include "forces.hpp"
#include "timestep.hpp"

struct Statistics {
    Real kinetic_energy;
    Real potential_energy;
    const size_t snapshotInterval;

    // Temporary buffers to hold those statistics until we write the state file
    std::vector<Real> KE_buffer;
    std::vector<Real> PE_buffer;

    Statistics (const InputBlock& Params) : 
        snapshotInterval(getParam<size_t>(Params, "snapshotInterval")) 
    {
        KE_buffer.resize(snapshotInterval, 0.0);
        PE_buffer.resize(snapshotInterval, 0.0);
    }

    void SetStats(Particles& P, Forces& F, Timestepper& T) {
        // Potential energy
        potential_energy = F.gravity.compute_potential_energy(P);

        // Kinetic energy
        kinetic_energy = compute_kinetic_energy(P);

        // Fill stat buffers
        fillBuffers(T);
    }

    Real compute_kinetic_energy(Particles& P);

    void fillBuffers(Timestepper& T) {
        size_t stat_index = T.step_ID % snapshotInterval; 
        KE_buffer[stat_index] = kinetic_energy;
        PE_buffer[stat_index] = potential_energy;
    }
};