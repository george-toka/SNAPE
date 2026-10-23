#include "forces.hpp"

Real Forces::getMinSofteningLength() {
    Real L_grav, L_hydro;
    Real max = std::numeric_limits<Real>::max();
    L_grav = max; L_hydro = max;

    gravity.getSofteningLength(L_grav);
    hydro.getSofteningLength(L_hydro);

    return std::min(L_grav, L_hydro);
}

void Forces::Acceleration(Particles& P) {
    // Flush data before accumulating values on registers
    P.flushData();

    gravity(P);
    hydro(P);
}