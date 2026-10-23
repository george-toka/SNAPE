#include "compile_macros.hpp"
#include "solver_macros.hpp"

#if GRAVITY_WRAPPER == WRAPPER_JAMES || HYDRO_WRAPPER == WRAPPER_JAMES

#include "james_algorithm.hpp"
#include <cmath>

void JamesSharedData::initialise_bound_dists(size_t N, const UniformMesh& D) {
    size_t Nd = N - 2;
    size_t f_off = Nd * Nd;
    
    // Pre-calculate the physical coordinates of every boundary node in order
    // This allows us to convert the complex 3D boundary navigation into a simple 1D array
    std::vector<Real> bx(N_bnd), by(N_bnd), bz(N_bnd);
    
    for (size_t boundary = 0; boundary < N_bnd; boundary++) {
        size_t face = boundary / f_off;
        size_t local_idx = boundary % f_off;
        
        size_t i0 = 0, j0 = 0, k0 = 0;
        if      (face == 0) { i0 = 1;   j0 = local_idx / Nd + 1; k0 = local_idx % Nd + 1; } // Front
        else if (face == 1) { i0 = N-2; j0 = local_idx / Nd + 1; k0 = local_idx % Nd + 1; } // Back
        else if (face == 2) { j0 = 1;   i0 = local_idx / Nd + 1; k0 = local_idx % Nd + 1; } // Down
        else if (face == 3) { j0 = N-2; i0 = local_idx / Nd + 1; k0 = local_idx % Nd + 1; } // Up
        else if (face == 4) { k0 = 1;   i0 = local_idx / Nd + 1; j0 = local_idx % Nd + 1; } // Right
        else if (face == 5) { k0 = N-2; i0 = local_idx / Nd + 1; j0 = local_idx % Nd + 1; } // Left

        bx[boundary] = D.mesh_points[0][i0];
        by[boundary] = D.mesh_points[1][j0];
        bz[boundary] = D.mesh_points[2][k0];
    }

    // Compute only the upper triangle of the geometric distance matrix
    // The matrix is symmetric (A[i][j] == A[j][i]). The diagonal is natively 0.
    for (size_t row = 0; row < N_bnd; row++) {
        for (size_t col = row + 1; col < N_bnd; col++) {
            Real dx = bx[row] - bx[col];
            Real dy = by[row] - by[col];
            Real dz = bz[row] - bz[col];
            
            Real r2 = dx*dx + dy*dy + dz*dz;
            Real inv_dist = (r2 > 0.0) ? (1.0 / std::sqrt(r2)) : 0.0;
            
            // Assign symmetrically, halving initialization time
            bound_dists[row * N_bnd + col] = inv_dist;
            bound_dists[col * N_bnd + row] = inv_dist;
        }
    }
}


void flush_boundaries(std::vector<Real>& x, size_t N) {
    size_t N2 = N * N;
    size_t n_off = N2 - N;
    size_t offset;

    // Front / Back faces: i = 0, N-1
    for (size_t j = 1; j < N - 1; j++) {
        offset = j * N;
        std::fill(x.begin() + offset + 1, x.begin() + offset + N - 1, 0.0);

        offset += (N - 1) * N2;
        std::fill(x.begin() + offset + 1, x.begin() + offset + N - 1, 0.0);
    }

    // Downward / Upward faces: j = 0, N-1
    for (size_t i = 1; i < N - 1; i++) {
        offset = i * N2;
        std::fill(x.begin() + offset + 1, x.begin() + offset + N - 1, 0.0);

        offset += n_off;

        std::fill(x.begin() + offset + 1, x.begin() + offset + N - 1, 0.0);
    }

    // Right / Left faces: k = 0, N-1
    n_off = N - 1;
    for (size_t i = 1; i < N - 1; i++) {
        for (size_t j = 1; j < N - 1; j++) {
            offset = i * N2 + j * N;

            x[offset] = 0.0;
            x[offset + n_off] = 0.0;
        }
    }
}

void extract_screening_charges(const std::vector<Real>& x, std::vector<Real>& b_s, size_t N, Real inv_denom) {
    const size_t N2 = N * N;

    // Read offsets (one cell inward from the boundaries)
    const size_t x_back_read = (N - 2) * N2;
    const size_t y_up_read   = (N - 2) * N;
    const size_t z_left_read = N - 2;

    size_t Nd = N-2;
    const size_t f_off = Nd * Nd;
    size_t bs_ind;

    // X-Faces (Front and Back)
    for (size_t j = 1; j < N - 1; j++) {
        for (size_t k = 1; k < N - 1; k++) {
            size_t base = j * N + k;
            bs_ind = (j-1) * Nd + (k-1);
            
            b_s[bs_ind] = x[base + N2] * inv_denom;          // Front (reads i = 1)
            b_s[f_off + bs_ind] = x[base + x_back_read] * inv_denom; // Back (reads i = N-2)
        }
    }

    size_t bs_off = 2*f_off;
    size_t off = bs_off + f_off;

    // Y-Faces (Downward and Upward)
    for (size_t i = 1; i < N - 1; i++) {
        for (size_t k = 1; k < N - 1; k++) {
            size_t base = i * N2 + k;
            bs_ind = (i-1) * Nd + (k-1);
            
            b_s[bs_off + bs_ind] = x[base + N] * inv_denom;           // Down (reads j = 1)
            b_s[off + bs_ind] = x[base + y_up_read] * inv_denom;   // Up (reads j = N-2)
        }
    }

    bs_off *= 2;
    off = bs_off + f_off;

    // Z-Faces (Right and Left)
    for (size_t i = 1; i < N - 1; i++) {
        for (size_t j = 1; j < N - 1; j++) {
            size_t base = i * N2 + j * N;
            bs_ind = (i-1) * Nd + (j-1);
            
            b_s[bs_off + bs_ind] = x[base + 1] * inv_denom;           // Right (reads k = 1)
            b_s[off + bs_ind] = x[base + z_left_read] * inv_denom; // Left (reads k = N-2)
        }
    }
}

void calculate_and_assign_potentials(UniformMesh& D, std::vector<Real>& x, const std::vector<Real>& b_s, const std::vector<Real>& inv_r_matrix, Real C) {
    const size_t N = D.N_total;
    const size_t N2 = N * N;
    const size_t Nd = N - 2;
    const size_t f_off = Nd * Nd;
    const size_t N_B = 6 * f_off;
    const Real coeff = -C * D.h * D.h * D.h;

    // Write offsets (absolute boundary faces)
    const size_t x_back_write = (N - 1) * N2;
    const size_t y_up_write   = (N - 1) * N;
    const size_t z_left_write = N - 1;

    // A fast lambda to do the dot product for a specific boundary cell 't'
    // This will be perfectly inlined by the compiler.
    auto compute_phi = [&](size_t t) -> Real {
        Real target_phi = 0.0;
        size_t row_offset = t * N_B;
        
        for (size_t s = 0; s < N_B; s++) {
            target_phi += b_s[s] * inv_r_matrix[row_offset + s];
        }
        return target_phi * coeff;
    };

    // X-Faces (Front and Back)
    for (size_t j = 1; j < N - 1; j++) {
        for (size_t k = 1; k < N - 1; k++) {
            size_t base = j * N + k;
            size_t local_idx = (j - 1) * Nd + (k - 1);
            
            x[base]                = compute_phi(local_idx);           // Front
            x[x_back_write + base] = compute_phi(f_off + local_idx);   // Back
        }
    }

    size_t bs_off = 2 * f_off;

    // Y-Faces (Downward and Upward)
    for (size_t i = 1; i < N - 1; i++) {
        for (size_t k = 1; k < N - 1; k++) {
            size_t base = i * N2 + k;
            size_t local_idx = (i - 1) * Nd + (k - 1);
            
            x[base]              = compute_phi(bs_off + local_idx);         // Down
            x[base + y_up_write] = compute_phi(bs_off + f_off + local_idx); // Up
        }
    }

    bs_off *= 2;

    // Z-Faces (Right and Left)
    for (size_t i = 1; i < N - 1; i++) {
        for (size_t j = 1; j < N - 1; j++) {
            size_t base = i * N2 + j * N;
            size_t local_idx = (i - 1) * Nd + (j - 1);
            
            x[base]                = compute_phi(bs_off + local_idx);         // Right
            x[base + z_left_write] = compute_phi(bs_off + f_off + local_idx); // Left
        }
    }
}

#endif