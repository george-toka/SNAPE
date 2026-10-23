#pragma once

#include "particles.hpp"
#include "io.hpp"
#include "domain_manager.hpp"
#include "solver_manager.hpp"
#include "forces.hpp"
#include "integrator.hpp"
#include "timestep.hpp"
#include "stats.hpp"

// ==========================================================================================
// ---test_folder : Relative path from project's root directory + the test's name
//                  e.g ./tests/halo_gravity_test/halo_gravity_test
// ---stateID     : used to retrieve full state file path by test_folder + "_state" + stateID
// ---receiptID   : used to retrieve full receipt file path ...
// ==========================================================================================
void Simulation(const std::string& base_path, size_t stateID, const int receiptID);