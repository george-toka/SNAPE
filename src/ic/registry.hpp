#pragma once

#include <functional>
#include "particles.hpp"

// Create a macro that invokes a lambda and executes immediately. use a bool ...
// dummy variable to be able to invoke the lambda and static so that there is no naming clash
#define REGISTER_GENERATOR(StringName, FunctionPtr) \
    static bool reg_##FunctionPtr = []() { \
        get_generator_map()[StringName] = FunctionPtr; \
        return true; \
    }();

// Build an alias using a generic wrapper function to group all (generator) ...
// functions that return void and take in Particles and InputBlock as arguments
using GenFunction = std::function<void(Particles&,const InputBlock&)>;

// Create a single universal map for generator functions => to avoid global ...
// objects, create a singleton and inline it to avoid multiple definition errors
inline std::unordered_map<std::string, GenFunction>& get_generator_map() {
    static std::unordered_map<std::string, GenFunction> GeneratorMap;
    return GeneratorMap;
}
