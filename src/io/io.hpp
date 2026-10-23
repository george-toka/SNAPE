#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <hdf5.h>
#include "compile_macros.hpp"
#include "io_macros.hpp"
#include "io_memreqs.hpp"

// ==========================================================================
// IO Tools to Save/Load state, export statistics & "bill" parameters in HDF5
// ==========================================================================
struct HDF5StatisticsExport
{
    std::string filename;

    HDF5StatisticsExport(const std::string& filename_new) :
        filename(filename_new)
    {}

    void Export(const DataRegistry& registry) const;
};

struct HDF5HandleRAII {
    hid_t file;

    explicit HDF5HandleRAII (hid_t raw_file) : file(raw_file) {}
    ~HDF5HandleRAII() {
        if (file >= 0) {
            H5Fclose(file);
        }
    }

    // Delete copy constructors to prevent double-closing the same file handle
    HDF5HandleRAII(const HDF5HandleRAII&) = delete;
    HDF5HandleRAII& operator=(const HDF5HandleRAII&) = delete;
};

void HDF5SaveState(const std::string& base_path, const DataRegistry& registry, const size_t N, const int stateID = 0, const Real t = 0);
void HDF5SaveParameterReceipt(const std::string& base_path, const InputBlock& InputParams, const int receiptID = 0);
void HDF5LoadState(const std::string& base_path, const int stateID, const DataRegistry& registry, size_t& N, Real& t);
void HDF5LoadParameterReceipt(const std::string& base_path, const int receiptID, InputBlock& SimParams);
InputBlock ReadRuntimeConfig(const std::string& build_path);
