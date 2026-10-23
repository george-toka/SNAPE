#include <iostream>
#include <random>
#include <cmath>
#include "generators.hpp"
#include "registry.hpp"
    
void PlummerSphere(Particles& P, const InputBlock& InputParams) {
    // ==================================================
    // GENERATE POSITIONS /W INVERSE TRANSFORM SAMPLING 
    // ==================================================
    // GENERATE VELOCITIES /W REJECTION SAMPLING
    // ==================================================
    
    const auto& [N, M, G] = unpackParams<size_t,Real,Real>(InputParams,"N","M","G");
    const auto& [R, seed, isVirialised] = unpackParams<Real,int,int>(InputParams,"UniformSphere.R","seed","UniformSphere.isVirialised");
    
    // Set seed for Mersenne Twister if given, else pull random seed from OS
    std::mt19937 mt;
    if (seed != -1) {
        mt.seed(seed);
    } else {
        std::random_device rd;
        mt.seed(rd());
    }
    
    // Initialise the sampling distribution
    std::uniform_real_distribution<double> U(0.0, 1.0);

    // Define the height of the bounding box for rejection sampling
    const double g_max = 0.1;
    const double PI = std::acos(-1.0);      

    // qr, qz hold the 'radial' and 'z' component of either position or velocity
    // X1 and X2 are placeholders for the samples we extract from MT
    for(size_t i = 0; i < N; i++) {
        // ================
        //    POSITIONS
        // ================

        // Sample X1 for radius - X1 can't be zero
        double X1 = 0.0;
        while(X1 == 0.0)
            X1 = U(mt);
        
        // Invert CDF to solve for radius r
        // Equation: r = R / sqrt(X1^(-2/3) - 1)
        double qr = R / std::sqrt(std::pow(X1, -2.0 / 3.0) - 1.0);
        
        // Draw isotropic angles
        // X2 = cosθ and X3 = φ / 2π so both range from [-1,1]
        X1 = U(mt); // I don't like extra doubles - reuse sample placeholders
        double X2 = U(mt);

        // Draw isotropic values for polar and azimuth
        double qz = (1.0 - 2.0 * X1) * qr;
        double temp1 = std::sqrt(qr * qr - qz * qz);
        double temp2 = 2 * PI * X2;

        // Direct Assignments
        P.positions[0][i] = temp1 * std::cos(temp2);
        P.positions[1][i] = temp1 * std::sin(temp2);
        P.positions[2][i] = qz;

        // ================
        //    VELOCITIES
        // ================
        if (!isVirialised)
        {
            P.velocities[0][i] = 0.0;
            P.velocities[1][i] = 0.0;
            P.velocities[2][i] = 0.0;

            continue;
        }

        double q = 0.0;

        while(true) {
            // Pull sample 
            X1 = U(mt); // Candidate q (x-axis)
            X2 = U(mt); // Height fraction (y-axis)

            // Calculate the actual Plummer velocity curve at X1
            // g(q) = q^2 * (1 - q^2)^3.5
            double X1_sq = X1 * X1;
            double t = 1.0 - X1_sq;
            double g_q = X1_sq * t*t*t*std::sqrt(t);

            // If sample is under the actual PDF curve, accept the sample
            if (g_max * X2 < g_q) {
                q = X1;
                break;
            }
        }

        // Define maximum velocity (Escape Velocity) for this specific radius
        double v_escape = std::sqrt(2.0 * G * M / std::sqrt((qr * qr) + (R * R)));
        qr = q * v_escape;

        // Sample again for isotropic angles (Reusing X1 and X2)
        X1 = U(mt); 
        X2 = U(mt); 

        qz = (1.0 - 2.0 * X1) * qr;
        temp1 = std::sqrt((qr * qr) - (qz * qz));
        temp2 = 2.0 * PI * X2;
        
        P.velocities[0][i] = temp1 * std::cos(temp2);
        P.velocities[1][i] = temp1 * std::sin(temp2);
        P.velocities[2][i] = qz;
    }
}


void UniformSphere(Particles& P, const InputBlock& InputParams)
{
    // ==================================================
    //    UNPACK PARAMETERS
    // ==================================================

    const auto& [N, M, G] = unpackParams<size_t,Real,Real>(InputParams,"N","M","G");
    const auto& [R, seed, isVirialised] = unpackParams<Real,int,int>(InputParams,"PlummerSphere.R","seed","PlummerSphere.isVirialised");

    // ==================================================
    //    INITIALISE RANDOM NUMBER GENERATOR
    // ==================================================

    std::mt19937 mt;

    if (seed != -1)
    {
        mt.seed(seed);
    }
    else
    {
        std::random_device rd;
        mt.seed(rd());
    }

    std::uniform_real_distribution<Real> U(0.0, 1.0);


    // ==================================================
    //    CONSTANTS
    // ==================================================

    const Real PI = std::acos(-1.0);


    // ==================================================
    //    GENERATE POSITIONS
    // ==================================================

    for (size_t i = 0; i < N; i++)
    {
        // Sample uniformly inside a sphere.
        //
        // r = R * u^(1/3)

        Real u = U(mt);
        Real r = R * std::cbrt(u);

        // Isotropic direction

        Real cosTheta = 1.0 - 2.0 * U(mt);
        Real phi = 2.0 * PI * U(mt);

        Real sinTheta =
            std::sqrt(
                std::max(
                    Real(0.0),
                    1.0 - cosTheta * cosTheta
                )
            );

        P.positions[0][i] =
            r * sinTheta * std::cos(phi);

        P.positions[1][i] =
            r * sinTheta * std::sin(phi);

        P.positions[2][i] =
            r * cosTheta;


        // ==================================================
        //    VELOCITIES
        // ==================================================

        if (!isVirialised)
        {
            P.velocities[0][i] = 0.0;
            P.velocities[1][i] = 0.0;
            P.velocities[2][i] = 0.0;

            continue;
        }

        // Generate arbitrary isotropic velocity direction.
        //
        // The magnitude will be scaled globally below.

        Real cosTheta_v = 1.0 - 2.0 * U(mt);
        Real phi_v = 2.0 * PI * U(mt);

        Real sinTheta_v =
            std::sqrt(
                std::max(
                    Real(0.0),
                    1.0 - cosTheta_v * cosTheta_v
                )
            );

        P.velocities[0][i] =
            sinTheta_v * std::cos(phi_v);

        P.velocities[1][i] =
            sinTheta_v * std::sin(phi_v);

        P.velocities[2][i] =
            cosTheta_v;
    }


    // ==================================================
    //    SCALE VELOCITIES TO VIRIAL EQUILIBRIUM
    // ==================================================

    if (isVirialised)
    {
        const Real particleMass = M / static_cast<Real>(N);

        // Gravitational potential energy of a uniform sphere
        const Real U_potential =
            -3.0 / 5.0 * G * M * M / R;

        // Virial condition:
        //
        // 2K + U = 0
        //
        const Real K_target =
            -0.5 * U_potential;

        // Current kinetic energy
        Real K_current = 0.0;

        for (size_t i = 0; i < N; i++)
        {
            Real vx = P.velocities[0][i];
            Real vy = P.velocities[1][i];
            Real vz = P.velocities[2][i];

            K_current +=
                0.5 * particleMass *
                (vx * vx + vy * vy + vz * vz);
        }

        // Scale all velocities by the same factor.
        const Real velocityScale =
            std::sqrt(K_target / K_current);

        for (size_t i = 0; i < N; i++)
        {
            P.velocities[0][i] *= velocityScale;
            P.velocities[1][i] *= velocityScale;
            P.velocities[2][i] *= velocityScale;
        }
    }
}

REGISTER_GENERATOR("UniformSphere", UniformSphere)
REGISTER_GENERATOR("PlummerSphere", PlummerSphere)