#pragma once

#include "particles.hpp"

// The fallback wrapper and solver must always be available
struct NoSolver {
};

struct NoWrapper : NoSolver {
    template <typename... Args> NoWrapper(Args&&...) {}
    void operator()(Particles&) {}
};