#pragma once
 
#include <tuple>
#include "parameters.hpp"

// ====================================================================
// Unpack Parameters from InputBlock (Base case & Pack Expansion)
// ====================================================================
template<typename T>
inline T getParam(const InputBlock& InputParams, const std::string& key)
{
    return std::get<T>(InputParams.at(key));
}

template<typename... T, typename... StringArgs>
std::tuple<T...> unpackParams(const InputBlock& InputParams, const StringArgs&... keys)
{
    static_assert(
        sizeof...(T) == sizeof...(StringArgs),
        "Number of parameter types must match number of keys"
    );

    return std::make_tuple(
        getParam<T>(InputParams, keys)... // Expand the directive multiple times based on the size of T
    );
}

// ==========================================================================
// Function that searches for the latest parameter receipt for a given prefix
// that came before the state file of interest
//      -> Returns the latest stepID
// ==========================================================================
int FindNearestInstanceID(const std::string& base_path, const std::string& prefix, const int start_id, const int interval = 1);