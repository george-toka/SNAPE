#pragma once

#include "particles.hpp"
#include "forces.hpp"
#include "timestep.hpp"

struct VelocityVerlet {
    void Step (Particles& P, Forces& F, Timestepper& timestepper);
};

struct RK2 {    
    void Step (Particles&, Forces&, Timestepper&);
};