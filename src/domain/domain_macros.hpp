#define NO_DOMAIN 0
#define UNIFORM_MESH 100

#ifndef MESH_SLOT_1
    #define MESH_SLOT_1 UNIFORM_MESH // Main shared pool
#endif
#ifndef MESH_SLOT_2
    #define MESH_SLOT_2 NO_DOMAIN    // An optional isolated pool
#endif