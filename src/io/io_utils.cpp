#include "io_utils.hpp"
#include <string>
#include <filesystem> 

int FindNearestInstanceID(const std::string& base_path, const std::string& prefix, const int start_id, const int interval) {
    int current_id = start_id;
    std::string base_filename = base_path + prefix;

    while (true) {
        std::string filepath = base_filename + std::to_string(current_id) + ".h5";
        
        if (std::filesystem::exists(filepath)) {
            return current_id;
        }
        
        if (current_id < interval) {
            break;
        }
        
        current_id -= interval;
    }
    
    throw std::runtime_error("[ERROR] No valid instance of " + base_filename + " found descending from step " + std::to_string(start_id));
}