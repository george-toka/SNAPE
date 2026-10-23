#pragma once

#include "domain.hpp"

struct DomainManager {
    // Physically allocate the engine's memory slots
    NoDomain void_slot;
    Slot1Type slot1;
    Slot2Type slot2;

    DomainManager(const InputBlock& Params, const Particles& P) :
        void_slot(Params, P, ""),
        slot1(Params, P, "MESH_SLOT_1."), 
        slot2(Params, P, "MESH_SLOT_2.") 
    {}

    void setupMeshes(const Particles& P) {
        slot1.setupMesh(P);
        slot2.setupMesh(P);
    }

    // ==========================================
    // ZERO-COST ROUTING
    // ==========================================
    auto& getGravityDomain() {
        if constexpr (GRAVITY_SLOT == 0) return void_slot;
        else if constexpr (GRAVITY_SLOT == 1) return slot1;
        else if constexpr (GRAVITY_SLOT == 2) return slot2;
    }

    auto& getHydroDomain() {
        if constexpr (HYDRO_SLOT == 0) return void_slot;
        if constexpr (HYDRO_SLOT == 1) return slot1;
        else if constexpr (HYDRO_SLOT == 2) return slot2;
    }
};