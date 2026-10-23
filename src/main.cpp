#include "ic_generation.hpp"
#include "simulation.hpp"
#include <filesystem>

int main(int argc, char* argv[]) {
    try {
        // Extract engine build path from argv[0]
        std::string engine_build = std::filesystem::path(argv[0]).parent_path().string();
        if (!engine_build.empty()) {
            engine_build += "/"; // Ensure it acts as a valid directory prefix
        }

        // Read configs using the extracted path
        InputBlock EngineActions = ReadRuntimeConfig(engine_build + "execution.txt");

        // Map execution configurations
        auto [action, test_folder, test_name, stepID] = unpackParams<std::string, std::string, std::string, size_t>(
            EngineActions, "action", "test_folder", "test_name", "stepID");

        // CLI Overrides (Highest priority: overrides text file if provided)
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--action" && i + 1 < argc) action = argv[++i];
            else if (arg == "--folder" && i + 1 < argc) test_folder = argv[++i];
            else if (arg == "--test" && i + 1 < argc) test_name = argv[++i];
            else if (arg == "--step" && i + 1 < argc) stepID = std::stoull(argv[++i]); 
        }

        std::string base_path = test_folder + test_name; // FULL PATH PREFIX
        InputBlock InputParams   = ReadRuntimeConfig(engine_build + "runtime_settings.txt");
        std::cout << getParam<std::string>(InputParams,"Generator");

        // ENGINE ACTION CONFIGURATION
        if (action == "ICGen") {
            ICGeneration(InputParams, base_path);
        }
        else if (action == "SimStandard") {
            // Find most recent parameter file and run
            std::string receipt_prefix = "_receipt";
            int receiptID = FindNearestInstanceID(base_path, receipt_prefix, stepID);
            
            Simulation(base_path, stepID, receiptID);
        }
        else if (action == "SimWithNewParams") {
            // Create new parameters and run with those
            HDF5SaveParameterReceipt(base_path, InputParams, stepID);

            Simulation(base_path, stepID, stepID);
        }
        else if (action == "ICGenNRun") {
            ICGeneration(InputParams, base_path);
            Simulation(base_path, 0, 0);
        }
        else {
            std::cout << "[WARNING] Choice for engine action is invalid." << "\n";
        }
    }

    catch (std::exception& e) {
        std::cerr << "Error caught during engine action" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return 0;
}
