#include "integrators.hpp"

void VelocityVerlet::Step(Particles& P, Forces& F, Timestepper& timestepper) {
    size_t N = P.N;
    Real dt = timestepper.dt; 

    // Half step for velocities
    for (size_t j = 0; j < 3; j++) {
        for (size_t i = 0; i < N; i++) {
            P.velocities[j][i] += 0.5 * P.accelerations[j][i] * dt;
        }
    }

    // Progress positions a full step with the above velocities
    for (size_t j = 0; j < 3; j++) {
        for (size_t i = 0; i < N; i++) {
            P.positions[j][i] += P.velocities[j][i] * dt;
        }
    }    

    // Find particle accelerations using staggered positions and velocities
    F.Acceleration(P);   

    // Synchronise velocities with the positions, by performing another half step
    for (size_t j = 0; j < 3; j++) {
        for (size_t i = 0; i < N; i++) {
            P.velocities[j][i] += 0.5 * P.accelerations[j][i] * dt;
        }
    }

}

void RK2::Step(Particles&, Forces&, Timestepper&) {}
