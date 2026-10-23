#include "ic_generation.hpp"

void ICGeneration(const InputBlock& InputParams, const std::string& base_path) {   
    // Initialise Particle System 
    Particles P(InputParams);
    P.allocate_initial();

    // Generate distribution
    generate_distribution(P, InputParams);

    // Register the filled data structures to be written in the HDF5 File
    DataRegistry registry;
    RegisterStateData(registry, P);

    // Save particle system state in HDF5 format    
    HDF5SaveState(base_path, registry, P.N);
    HDF5SaveParameterReceipt(base_path, InputParams);
}