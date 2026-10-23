// CIC: EITHER OVERLOAD OR USE TEMPLATE OR USE NESTED STRUCTS FOR MASS, CHARGE ETC TO PASS THOSE IN THE FUNCTION AND USE THEIR GETTERS
// CIC: ALSO NEEDS REFACTORING TO AVOID CACHE THRASHING -> SORT PARTICLES OR DOMAIN DECOMP (SORT OF)
#include "compile_macros.hpp"
#include "domain_macros.hpp"

#if MESH_SLOT_1 == UNIFORM_MESH || MESH_SLOT_2 == UNIFORM_MESH

#include "uniform_mesh.hpp"
#include <algorithm>
#include <cmath>


void UniformMesh::setupMesh(const Particles& P) {
    L = 0;

    // Keep the larger extent L = (max_ext - min_ext) from the 3 axii
    for (int i = 0; i < 3; i++) {
        domain_min[i] = *std::min_element(P.positions[i].begin(), P.positions[i].end()); // maybe i can optimise by finding both in one sweep??
        domain_max[i] = *std::max_element(P.positions[i].begin(), P.positions[i].end());
        domain_center[i] = (domain_min[i] + domain_max[i]) / 2;

        L = std::max(L, domain_max[i] - domain_min[i]);
    }
    // Define initial spacing
    h = L / N_mesh;
    inv_h = 1 / h;
    inv_h3 = 1 / (h*h*h);

    // Pad the domain with N_pad
    L += 2*N_pad*h; // "2" factor because we pad from both sides in each axis 

    // Center bounding domain based on axii geometric centers
    for (int i = 0; i < 3; i++) {
        domain_min[i] = domain_center[i] - L / 2;
        domain_max[i] = domain_center[i] + L / 2;
    }
    
    // Define mesh points for CELL-CENTERED SCHEME
    for (size_t i = 0; i < N_total-1; i++) {
        for (int j = 0; j < 3; j++) 
            mesh_points[j][i] = domain_min[j] + (i + 0.5)*h; 
    }

}


void UniformMesh::CIC_P2M(const Particles& P, Real interpolator, std::vector<Real>& interpolant) {
    // For each particle, find its cell -> anchor point -> neighboring cells -> find weights and interpolate
    Real dists[3];
    int cell_idx[3], neighbor_offset[3];
    
    // For each particle
    for (size_t i = 0; i < P.N; i++) {

        // Find the cell that it resides
        for (int j = 0; j < 3; j++)
            cell_idx[j] = static_cast<int>((P.positions[j][i] - domain_min[j]) * inv_h); // Essentially a faster floor() alternative 

        // Find the anchor point
        for (int j = 0; j < 3; j++) {
            dists[j] = P.positions[j][i] - mesh_points[j][cell_idx[j]];
            neighbor_offset[j] = (dists[j] > 0) * 2 - 1;
        }

        // Run through each neighbor where the whole neighbor list is [i,j,k], [i+offset,j,k], [i,j+offset,k] ... 8 of them
        int i_off_neighbor, j_off_neighbor, k_off_neighbor, offset;
        size_t base_idx = cell_idx[0]*N2_total + cell_idx[1]*N_total + cell_idx[2];

        for (int in = 0; in < 2; in++) {
            for (int jn = 0; jn < 2; jn++) {
                for (int kn = 0; kn < 2; kn++) {
                    i_off_neighbor = neighbor_offset[0] * in;
                    j_off_neighbor = neighbor_offset[1] * jn;
                    k_off_neighbor = neighbor_offset[2] * kn;
                    
                    offset = i_off_neighbor * N2_total + j_off_neighbor * N_total + k_off_neighbor;

                    // Could also calculate each distance by loading neighbor mesh points -> SoA layout i have rocks for this type of loop
                    Real w = ((h - dists[0]) + (2*dists[0] - h) * i_off_neighbor) *
                             ((h - dists[1]) + (2*dists[1] - h) * j_off_neighbor) *
                             ((h - dists[2]) + (2*dists[2] - h) * k_off_neighbor) *
                             inv_h3;

                    interpolant[base_idx + offset] += w * interpolator; // This one needs to be generalised. do we interpolate with constant or adaptive interpolator
                }
            }
        }

    }
}

#endif