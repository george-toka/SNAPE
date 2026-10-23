#pragma once

#include "parameters.hpp"
#include "compile_macros.hpp"
#include "particles.hpp"
#include "no_force.hpp"
#include "force_macros.hpp"

static_assert(GRAVITY_ENABLED || HYDRO_ENABLED, "[COMPILATION ERROR] At least one force module must be enabled");

// Set aliases for each force 
#if GRAVITY_ENABLED
    #include "gravity.hpp"
#else 
    using Gravity = EmptyForce;
#endif 

#if HYDRO_ENABLED 
    #include "hydro.hpp"
#else 
    using Hydro = EmptyForce;
#endif 




