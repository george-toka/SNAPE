#pragma once

#include <iostream>
#include <variant>
#include <unordered_map>
#include "compile_macros.hpp"

#ifndef REAL
    #define REAL double;
#endif

// CT(COMPILE TIME) CORE PARAMETERS AND ALIASES
using Real = REAL;

// ParamValue is a type-safe union that will be used as the value of a key in the unordered map InputBlock
using ParamValue = std::variant<int, size_t, Real, std::string, bool>;
// InputBlock is a dictionary where: key is the category (string) and value is the ParamValue
using InputBlock = std::unordered_map<std::string, ParamValue>;
