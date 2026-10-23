// Define solver ID's
#define DIRECT_SUMS 10
#define GRAVITY_MESH 11

// Make sure we define the gravity solver selector
#ifndef GRAVITY_FRAMEWORK
    #define GRAVITY_FRAMEWORK DIRECT_SUMS
#endif