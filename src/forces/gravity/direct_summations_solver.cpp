#include "compile_macros.hpp"
#include "gravity_macros.hpp"

#if GRAVITY_FRAMEWORK == DIRECT_SUMS

#include "direct_summations_solver.hpp"
#include <iostream>
#include <cmath>

void DirectSum::operator() (Particles& P) {
    size_t i_offset, triang_index;
    Real dx, dy, dz, ax, ay, az, r_sq, denom;
    Real m = P.m_particle; Real N = P.N;
    Real e_soft_sq = e_soft * e_soft;
    Real factor = G * m;

    for (size_t i = 0; i < N; i++) {
        i_offset = (i * (i - 1)) / 2;
        
        for (size_t j = i + 1; j < N; j++) {
            // Find pairwise distances per dimension
            dx = P.positions[0][i] - P.positions[0][j];
            dy = P.positions[1][i] - P.positions[1][j];
            dz = P.positions[2][i] - P.positions[2][j];

            // Find the squared radial distance of particles i and j
            r_sq = dx*dx + dy*dy + dz*dz;
            denom = 1 / (r_sq + e_soft_sq);
            triang_index = (j - i - 1) + i * (N - 1) - i_offset;
            inv_smoothed_distance[triang_index] = std::sqrt(denom);
            denom = - inv_smoothed_distance[triang_index] * denom;

            // Calculate the acceleration per dimension
            ax = dx * denom;
            ay = dy * denom;
            az = dz * denom;

            P.accelerations[0][i] += ax;
            P.accelerations[1][i] += ay;
            P.accelerations[2][i] += az;

            P.accelerations[0][j] -= ax;
            P.accelerations[1][j] -= ay;
            P.accelerations[2][j] -= az;
        }

        P.accelerations[0][i] *= factor;
        P.accelerations[1][i] *= factor;
        P.accelerations[2][i] *= factor;
    }

}

#endif