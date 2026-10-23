#include "generate_distribution.hpp"

void generate_distribution(Particles& P, const InputBlock& InputParams) {
    std::string GeneratorFunction = getParam<std::string>(InputParams,"Generator");
    
    // Fetch the already filled map
    const auto& map = get_generator_map();

    // Returns an iterator of the unordered_map
    const auto it = map.find(GeneratorFunction);

    // Fail if iterator reached the end
    if (it == map.end()) 
        throw std::runtime_error("[ERROR] Invalid Generator");

    // Dispatch function by parsing the key and using operator()
    std::cout << "[INIT] Routing to generator: " << GeneratorFunction << "...\n";
    it -> second(P, InputParams);
    std::cout << "[SUCCESS] Succesful generation of: " << GeneratorFunction << "\n";
}