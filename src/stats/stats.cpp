#include "stats.hpp"

Real Statistics::compute_kinetic_energy(Particles& P) {
    Real sum_v2 = 0;
    for (size_t i = 0; i < P.N; i++) {
        P.v2[i] = P.velocities[0][i]*P.velocities[0][i] + P.velocities[1][i]*P.velocities[1][i] + P.velocities[2][i]*P.velocities[2][i];
        sum_v2 += P.v2[i];
    }

    return 0.5 * P.m_particle * sum_v2;
}