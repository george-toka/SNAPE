#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>
#include "particles.hpp"
#include "forces.hpp"

struct TimestepConditionFlags {
    bool CFL_flag;
    bool FORCE_flag;

    TimestepConditionFlags(bool use_cflcond, bool use_forcecond) 
        : CFL_flag(use_cflcond), FORCE_flag(use_forcecond)
    {}
};

struct Timestepper {
    Real t;
    Real dt;
    size_t step_ID;
    const Real t_end;
    const TimestepConditionFlags condition_flags{1,1}; // Will be determined by InputBlock/compile time Configuration as well in the future
    const Real MAX_DT = 100000;
    const Real CFL_NUM = 0.7; // Will be determined by InputBlock as well in the future
    const Real FORCE_NUM = 0.7; // Will be determined by InputBlock as well in the future
    const Real EPSILON = 0.001; // Will be determined by InputBlock as well in the future

    Timestepper(const InputBlock& Params, size_t stateID) :
        t(0.0),
        dt(0.0),
        step_ID(stateID), 
        t_end(getParam<Real>(Params, "t_end"))
    {}
    
    void AdvanceClock() {
        this->t += this->dt;
        this->step_ID++;
    }

    // -------------------
    // CRITERIA CAN CHANGE 
    // BASED ON FORCES
    // -------------------
    void TimesteppingCriteria(Particles& P, Forces& F) {
        dt = MAX_DT;
        Real a2, t_acc, t_cfl;
        Real L_SOFT = F.getMinSofteningLength();        
        
        for (size_t i = 0; i < P.N; i++) {
            a2 = P.accelerations[0][i]*P.accelerations[0][i] + P.accelerations[1][i]*P.accelerations[1][i] + P.accelerations[2][i]*P.accelerations[2][i];
            
            t_acc = condition_flags.FORCE_flag * (FORCE_NUM * std::sqrt(L_SOFT / (std::sqrt(a2) + EPSILON))) 
                    + (1 - condition_flags.FORCE_flag) * MAX_DT;
                    
            t_cfl = condition_flags.CFL_flag * (CFL_NUM * L_SOFT / (std::sqrt(P.v2[i]) + EPSILON)) 
                    + (1 - condition_flags.CFL_flag) * MAX_DT;

            dt = std::min({dt, t_acc, t_cfl});
        }

    }
};