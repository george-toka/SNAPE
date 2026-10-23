#include "io_memreqs.hpp"

// In the future this can be decentralised and each module that wants to subscribe its data in either registry just exposes
// a function/interface register_state_data()
void RegisterStateData(DataRegistry& registry, Particles& P) {
    // 3D Vectors
    registry["Positions"]  = std::array<std::vector<Real>*, 3>{&P.positions[0], &P.positions[1], &P.positions[2]};
    registry["Velocities"] = std::array<std::vector<Real>*, 3>{&P.velocities[0], &P.velocities[1], &P.velocities[2]};

    // 1D Vectors

    // hydro ... gravity ... vectors
}

void RegisterStatistics(DataRegistry& registry, Statistics& S) {
    // 1D Vectors of size "snapshotInterval"
    registry["Kinetic Energy"] = &S.KE_buffer;
    registry["Potential Energy"] = &S.PE_buffer;
}