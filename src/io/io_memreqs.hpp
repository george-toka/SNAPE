#pragma once

#include "parameters.hpp"
#include "particles.hpp"
#include "stats.hpp"
#include <array>

using dataset_memtype = std::variant<std::vector<Real>*, std::array<std::vector<Real>*,3>>;
using DataRegistry = std::unordered_map<std::string, dataset_memtype>;

void RegisterStateData(DataRegistry& registry, Particles& P);
void RegisterStatistics(DataRegistry& registry, Statistics& stats);