// Define ID's for each integrator
#define RK_2 1000
#define VELOCITY_VERLET 1001

// Make sure the integrator selector is defined
#ifndef INTEGRATOR
    #define INTEGRATOR VELOCITY_VERLET 
#endif