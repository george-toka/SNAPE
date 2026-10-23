#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <fstream>
#include "io.hpp"
#include "io_utils.hpp"
#include "io_memreqs.hpp"

// ==========================================================================
// Implementation of the IO HDF5 Functions
// ==========================================================================
void HDF5SaveState(const std::string& base_path, const DataRegistry& registry, size_t N, const int stateID, const Real t) {
    // Create an HDF5 File
    std::string filename = base_path + "_state" + std::to_string(stateID) + ".h5";
    hid_t file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    if (file == H5I_INVALID_HID)
        throw std::runtime_error("[ERROR] Failed to create HDF5 file: " + filename);

    // Wrap immediately
    HDF5HandleRAII file_guard(file);

    // Create group
    hid_t particle_group = H5Gcreate2(file, "/Particles", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (particle_group == H5I_INVALID_HID) {
        throw std::runtime_error("[ERROR] Failed to create HDF5 particle group");
    }

    // Create Dataspaces
    hsize_t dim_scalar[1] = { static_cast<hsize_t>(N) };
    hid_t space_scalar = H5Screate_simple(1, dim_scalar, NULL);

    hsize_t dim_vector[2] = { static_cast<hsize_t>(N), 3 };
    hid_t space_vector = H5Screate_simple(2, dim_vector, NULL);

    if (space_scalar == H5I_INVALID_HID || space_vector == H5I_INVALID_HID) {
        // ... cleanup and throw ...
        throw std::runtime_error("[ERROR] Failed to create HDF5 dataspaces");
    }

    // Iterate through the dynamic registry and write everything automatically
    for (const auto& [name, data_variant] : registry) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            // --- SCALAR (1D) ---
            if constexpr (std::is_same_v<T, std::vector<Real>*>) {
                hid_t dataset = H5Dcreate2(particle_group, name.c_str(), HDF5_RealType, space_scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                if (H5Dwrite(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, arg->data()) < 0)
                    throw std::runtime_error("[ERROR] Failed to write HDF5 dataset: " + name);
                H5Dclose(dataset);
            } 
            
            // --- VECTOR (3D) ---
            else if constexpr (std::is_same_v<T, std::array<std::vector<Real>*, 3>>) {
                hid_t dataset = H5Dcreate2(particle_group, name.c_str(), HDF5_RealType, space_vector, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                
                // Pack SoA to AoS
                std::vector<Real> packed_data(N * 3);
                
                for (size_t i = 0; i < N; ++i) {
                    packed_data[3 * i + 0] = (*arg[0])[i];
                    packed_data[3 * i + 1] = (*arg[1])[i];
                    packed_data[3 * i + 2] = (*arg[2])[i];
                }

                if (H5Dwrite(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, packed_data.data()) < 0)
                    throw std::runtime_error("[ERROR] Failed to write HDF5 dataset: " + name);
                
                H5Dclose(dataset);
            }
        }, data_variant);
    }

    // Attach Attributes
    hid_t attr_space = H5Screate(H5S_SCALAR);

    hid_t attr_step = H5Acreate2(particle_group, "stepID", H5T_NATIVE_INT, attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_step, H5T_NATIVE_INT, &stateID);
    H5Aclose(attr_step);

    hid_t attr_time = H5Acreate2(particle_group, "time", HDF5_RealType, attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_time, HDF5_RealType, &t);
    H5Aclose(attr_time);

    // Cleanup
    H5Sclose(attr_space);
    H5Sclose(space_scalar);
    H5Sclose(space_vector);
    H5Gclose(particle_group);

    //std::cout << "[SUCCESS] HDF5 State File saved successfully: " << filename << "\n";
}


void HDF5SaveParameterReceipt(const std::string& base_path, const InputBlock& InputParams, const int receiptID) {
    // Create file and open root group
    std::string filename = base_path + "_receipt" + std::to_string(receiptID) + ".h5";
    hid_t file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    if (file == H5I_INVALID_HID)
        throw std::runtime_error("[ERROR] Failed to create HDF5 receipt file: " + filename);

    // Wrap immediately
    HDF5HandleRAII file_guard(file);

    hid_t root_group = H5Gopen2(file, "/", H5P_DEFAULT);
    if (root_group == H5I_INVALID_HID) {
        throw std::runtime_error("[ERROR] Failed to open HDF5 root group in: " + filename);
    }

    hid_t attr_space = H5Screate(H5S_SCALAR);

    // Iterate through the unordered_map
    for (const auto& [key, val] : InputParams) {
        
        // std::visit unpacks the std::variant at compile time
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            hid_t attr_type;

            // Handle all numeric types in one block
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, size_t> || std::is_same_v<T, Real>) {
                if constexpr (std::is_same_v<T, int>)         attr_type = H5T_NATIVE_INT;
                else if constexpr (std::is_same_v<T, size_t>) attr_type = H5T_NATIVE_UINT64;
                else if constexpr (std::is_same_v<T, Real>)   attr_type = HDF5_RealType;

                hid_t attr = H5Acreate2(root_group, key.c_str(), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                H5Awrite(attr, attr_type, &arg);
                H5Aclose(attr);
            } 
            // Isolate string handling
            else if constexpr (std::is_same_v<T, std::string>) {
                attr_type = H5Tcopy(H5T_C_S1);
                H5Tset_size(attr_type, arg.size() + 1);
                H5Tset_strpad(attr_type, H5T_STR_NULLTERM);
                
                hid_t attr = H5Acreate2(root_group, key.c_str(), attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
                H5Awrite(attr, attr_type, arg.c_str());
                
                H5Aclose(attr);
                H5Tclose(attr_type);
            }
        }, val);
    }

    H5Sclose(attr_space);
    H5Gclose(root_group);

    //std::cout << "[SUCCESS] HDF5 Parameter Receipt File saved successfully" << "\n";
}


void HDF5LoadState(const std::string& base_path, const int stateID, const DataRegistry& registry, size_t& N, Real& t) {
    std::string filename = base_path + "_state" + std::to_string(stateID) + ".h5";
    hid_t file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    if (file == H5I_INVALID_HID)
        throw std::runtime_error("[ERROR] Failed to open HDF5 state file: " + filename);

    // Wrap immediately
    HDF5HandleRAII file_guard(file);

    hid_t particle_group = H5Gopen2(file, "/Particles", H5P_DEFAULT);
    if (particle_group == H5I_INVALID_HID) {
        throw std::runtime_error("[ERROR] Failed to open HDF5 particle group in: " + filename);
    }

    // Read Attributes (time and stepID)
    hid_t attr_time = H5Aopen(particle_group, "time", H5P_DEFAULT);
    if (attr_time != H5I_INVALID_HID) {
        H5Aread(attr_time, HDF5_RealType, &t);
        H5Aclose(attr_time);
    }

    // Discover N dynamically by inspecting the first valid dataset in the registry
    hid_t dataset_for_n = H5I_INVALID_HID;
    for (const auto& [name, data_variant] : registry) {
        dataset_for_n = H5Dopen2(particle_group, name.c_str(), H5P_DEFAULT);
        if (dataset_for_n != H5I_INVALID_HID) break;
    }
    
    if (dataset_for_n == H5I_INVALID_HID) {
        H5Gclose(particle_group);
        H5Fclose(file);
        throw std::runtime_error("[ERROR] Could not find any valid datasets to discover N.");
    }

    hid_t space_for_n = H5Dget_space(dataset_for_n);
    hsize_t dims[2];
    H5Sget_simple_extent_dims(space_for_n, dims, NULL);
    N = dims[0]; // Set N dynamically based on the file dimensions
    
    H5Sclose(space_for_n);
    H5Dclose(dataset_for_n);

    // Iterate through the registry, resize memory, and load data
    for (const auto& [name, data_variant] : registry) {
        hid_t dataset = H5Dopen2(particle_group, name.c_str(), H5P_DEFAULT);
        
        if (dataset == H5I_INVALID_HID) {
            std::cout << "[WARNING] Dataset '" << name << "' not found in file. Skipping.\n";
            continue;
        }

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            // --- SCALAR (1D) ---
            if constexpr (std::is_same_v<T, std::vector<Real>*>) {
                arg->resize(N); // The IO pipeline resizes the struct memory automatically!
                if (H5Dread(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, arg->data()) < 0)
                    throw std::runtime_error("[ERROR] Failed to read HDF5 dataset: " + name);
            } 
            
            // --- VECTOR (3D) ---
            else if constexpr (std::is_same_v<T, std::array<std::vector<Real>*, 3>>) {
                (*arg[0]).resize(N);
                (*arg[1]).resize(N);
                (*arg[2]).resize(N);
                
                std::vector<Real> packed_data(N * 3);
                if (H5Dread(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, packed_data.data()) < 0)
                    throw std::runtime_error("[ERROR] Failed to read HDF5 dataset: " + name);
                
                // Unpack AoS to SoA
                for (size_t i = 0; i < N; ++i) {
                    (*arg[0])[i] = packed_data[3 * i + 0];
                    (*arg[1])[i] = packed_data[3 * i + 1];
                    (*arg[2])[i] = packed_data[3 * i + 2];
                }
            }
        }, data_variant);
        
        H5Dclose(dataset);
    }

    H5Gclose(particle_group);

    std::cout << "[SUCCESS] Loaded State " << stateID << " | N = " << N << " | t = " << t << "\n";
}


void HDF5LoadParameterReceipt(const std::string& base_path, const int receiptID, InputBlock& params) {
    std::string filename = base_path + "_receipt" + std::to_string(receiptID) + ".h5";
    hid_t file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    
    if (file == H5I_INVALID_HID) 
        throw std::runtime_error("[ERROR] Cannot open file: " + filename);

    // Wrap immediately
    HDF5HandleRAII file_guard(file);

    hid_t root_group = H5Gopen(file, "/", H5P_DEFAULT);

    // Get the total number of attributes attached to the root group
    H5O_info_t infobuf;
    H5Oget_info(root_group, &infobuf, H5O_INFO_NUM_ATTRS); 
    
    // Loop through every attribute by index
    for (hsize_t i = 0; i < infobuf.num_attrs; i++) {
        // Open attribute by index (sorting by name alphanumerically)
        hid_t attr = H5Aopen_by_idx(root_group, ".", H5_INDEX_NAME, H5_ITER_INC, i, H5P_DEFAULT, H5P_DEFAULT);

        // Extract the key (Attribute Name)
        ssize_t name_len = H5Aget_name(attr, 0, nullptr);
        std::string key(name_len, '\0');
        H5Aget_name(attr, name_len + 1, key.data());

        // Extract the datatype and map it to C++ variant
        hid_t attr_type = H5Aget_type(attr);
        H5T_class_t type_class = H5Tget_class(attr_type);
        
        if (H5Tequal(attr_type, H5T_NATIVE_INT) > 0) {
            int val;
            H5Aread(attr, H5T_NATIVE_INT, &val);
            params[key] = val;
        } 
        else if (H5Tequal(attr_type, H5T_NATIVE_UINT64) > 0) {
            size_t val;
            H5Aread(attr, H5T_NATIVE_UINT64, &val);
            params[key] = val;
        } 
        else if (H5Tequal(attr_type, HDF5_RealType) > 0) { 
            Real val;
            H5Aread(attr, HDF5_RealType, &val);
            params[key] = val;
        } 
        else if (type_class == H5T_STRING) {
            // Strings require querying the exact string length allocated on disk
            size_t str_size = H5Tget_size(attr_type);
            std::string val(str_size, '\0');
            
            // Create a memory datatype to read the string into C++ memory safely
            hid_t mem_type = H5Tcopy(H5T_C_S1);
            H5Tset_size(mem_type, str_size);
            
            H5Aread(attr, mem_type, val.data());
            
            // Strip any trailing null terminators HDF5 might have saved
            val.erase(std::find(val.begin(), val.end(), '\0'), val.end());
            params[key] = val;
            
            H5Tclose(mem_type);
        }
        
        H5Tclose(attr_type);
        H5Aclose(attr);
    }

    H5Gclose(root_group);

    std::cout << "[SUCCESS] HDF5 Parameter Receipt File loaded successfully" << "\n";
}


void HDF5StatisticsExport::Export(const DataRegistry& registry) const {
    if (registry.empty()) return;

    // Create HDF5 file
    hid_t file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file < 0) {
        throw std::runtime_error("[IO ERROR] Failed to create statistics HDF5 file");
    }

    // Wrap immediately
    HDF5HandleRAII file_guard(file);

    // Determine number of samples (assuming all buffers share the same size)
    hsize_t N = 0;
    std::visit([&N](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<Real>*>) {
            N = static_cast<hsize_t>(arg->size());
        } else if constexpr (std::is_same_v<T, std::array<std::vector<Real>*, 3>>) {
            N = static_cast<hsize_t>(arg[0]->size()); // Size of the X component
        }
    }, registry.begin()->second);

    // Create one-dimensional dataspace
    hsize_t dims[1] = {N};
    hid_t dataspace = H5Screate_simple(1, dims, nullptr);
    if (dataspace < 0) {
        throw std::runtime_error("[IO ERROR] Failed to create statistics dataspace");
    }

    // Iterate over the registry and write each dataset automatically
    for (const auto& [dataset_name, variant_data] : registry) {
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::vector<Real>*>) {
                // Write 1D Vector (Kinetic Energy, Potential Energy)
                hid_t dataset = H5Dcreate2(file, dataset_name.c_str(), HDF5_RealType, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                H5Dwrite(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, arg->data());
                H5Dclose(dataset);
                
            } else if constexpr (std::is_same_v<T, std::array<std::vector<Real>*, 3>>) {
                // Write 3D Vector Arrays (Positions, Velocities)
                std::string axes[3] = {"_x", "_y", "_z"};
                for (int i = 0; i < 3; ++i) {
                    std::string comp_name = dataset_name + axes[i];
                    hid_t dataset = H5Dcreate2(file, comp_name.c_str(), HDF5_RealType, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                    H5Dwrite(dataset, HDF5_RealType, H5S_ALL, H5S_ALL, H5P_DEFAULT, arg[i]->data());
                    H5Dclose(dataset);
                }
            }
        }, variant_data);
    }

    // Close shared HDF5 objects
    H5Sclose(dataspace);
}


InputBlock ReadRuntimeConfig(const std::string& filepath) {
    InputBlock configMap;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR] Could not open configuration file: " + filepath);
    }

    std::string line;
    while (std::getline(file, line)) {
        
        // Skip empty lines and leading spaces
        size_t first_char = line.find_first_not_of(" \t\r");
        if (first_char == std::string::npos || line[first_char] == '#') {
            continue;
        }
        std::string clean_line = line.substr(first_char);

        // Extract the Type
        size_t space_idx = clean_line.find_first_of(" \t"); 
        if (space_idx == std::string::npos) {
            throw std::runtime_error("[ERROR] Missing type space in line: " + clean_line);
        }
        std::string type_str = clean_line.substr(0, space_idx);
        std::string equation = clean_line.substr(space_idx + 1);

        // Extract the Key and Value at the '='
        size_t equal_idx = equation.find('=');
        if (equal_idx == std::string::npos) {
            throw std::runtime_error("[ERROR] Missing '=' in line: " + clean_line);
        }
        
        std::string key = equation.substr(0, equal_idx);
        std::string remainder = equation.substr(equal_idx + 1);

        // Clean up the key (remove trailing spaces before '=')
        size_t key_end = key.find_last_not_of(" \t");
        if (key_end != std::string::npos) {
            key = key.substr(0, key_end + 1);
        }

        // Extract the Value safely, ignoring inline comments
        size_t val_start = remainder.find_first_not_of(" \t");
        if (val_start == std::string::npos) {
            throw std::runtime_error("[ERROR] Missing value after '=' in line: " + clean_line);
        }

        std::string value;
        if (remainder[val_start] == '"') {
            // String literal: Extract everything strictly between the quotes
            size_t closing_quote = remainder.find('"', val_start + 1);
            if (closing_quote == std::string::npos) {
                throw std::runtime_error("[ERROR] Missing closing quote in line: " + clean_line);
            }
            value = remainder.substr(val_start + 1, closing_quote - val_start - 1);
        } 
        else {
            // Standard value: Stop at the first space, tab, newline, or inline comment '#'
            size_t val_end = remainder.find_first_of(" \t\r\n#", val_start);
            value = remainder.substr(val_start, val_end - val_start);
        }

        // Safely cast to the correct C++ type
        if (type_str == "size_t") {
            configMap[key] = static_cast<size_t>(std::stoull(value));
        } 
        else if (type_str == "REAL") {
            configMap[key] = static_cast<REAL>(std::stod(value)); 
        } 
        else if (type_str == "int") {
            configMap[key] = std::stoi(value);
        } 
        else if (type_str == "bool") {
            configMap[key] = (value == "true" || value == "1");
        } 
        else {
            configMap[key] = value; 
        }
    }
    
    return configMap;
}
