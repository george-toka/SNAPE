#pragma once

#include "compile_macros.hpp"
#include "particles.hpp"
#include "domain_macros.hpp"
#include "force_macros.hpp"
#include "no_domain.hpp"

// ==========================================
// 1. ENGINE MEMORY SLOTS
// Select what physically exists in memory.
// ==========================================
template<int DomainID> struct DomainSelector : std::false_type {};
template <> struct DomainSelector<NO_DOMAIN> : std::true_type { using type = NoDomain; };

// Conditionally include ONLY what is actively assigned to a slot
#if MESH_SLOT_1 == UNIFORM_MESH || MESH_SLOT_2 == UNIFORM_MESH
    #include "uniform_mesh.hpp"
    template <> struct DomainSelector<UNIFORM_MESH> : std::true_type { using type = UniformMesh; };
#endif

static_assert(DomainSelector<MESH_SLOT_1>::value, "[COMPILATION ERROR] Invalid Domain ID assigned to MESH_SLOT_1");
static_assert(DomainSelector<MESH_SLOT_2>::value, "[COMPILATION ERROR] Invalid Domain ID assigned to MESH_SLOT_2");

using Slot1Type = typename DomainSelector<MESH_SLOT_1>::type;
using Slot2Type = typename DomainSelector<MESH_SLOT_2>::type;

// ==========================================
// 3. PHYSICS SUBSCRIPTIONS
// Solvers just point to a slot. 
// ==========================================
#ifndef GRAVITY_SLOT
    #define GRAVITY_SLOT 0
#endif
#ifndef HYDRO_SLOT
    #define HYDRO_SLOT 0
#endif

// Intercept disabled forces and route them to the Void Slot
#if !GRAVITY_ENABLED
    #undef GRAVITY_SLOT
    #define GRAVITY_SLOT 0
#endif

#if !HYDRO_ENABLED
    #undef HYDRO_SLOT
    #define HYDRO_SLOT 0
#endif


// Map the subscriptions to the final types
template<int SlotID> struct SlotSelector : std::false_type {};
template <> struct SlotSelector<0> : std::true_type { using type = NoDomain; }; // The void slot for EmptyForce
template <> struct SlotSelector<1> : std::true_type{ using type = Slot1Type; };
template <> struct SlotSelector<2> : std::true_type{ using type = Slot2Type; };

static_assert(SlotSelector<GRAVITY_SLOT>::value, "[COMPILATION ERROR] Gravity assigned to an invalid or unallocated memory slot");
static_assert(SlotSelector<HYDRO_SLOT>::value, "[COMPILATION ERROR] Hydro assigned to an invalid or unallocated memory slot");

using GravityDomain = typename SlotSelector<GRAVITY_SLOT>::type;
using HydroDomain   = typename SlotSelector<HYDRO_SLOT>::type;


