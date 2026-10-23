#include "simulation.hpp"

void Simulation(const std::string& base_path, size_t stateID, const int receiptID) {
    // ------------- SETUP SIMULATION PHASE -------------

    // Load parameters from receipt and distribute them to all modules
    InputBlock SimParams;
    HDF5LoadParameterReceipt(base_path, receiptID, SimParams);

    // Parameter Distribution logic by dividing assignments in respective top-level modules
    Particles P(SimParams); P.allocate_state();  
    Integrator integrator; Timestepper timestepper(SimParams, stateID); 

    // Create the registry mapping
    DataRegistry load_registry; RegisterStateData(load_registry, P);
    // HDF5LoadState discovers N, fills P's datasets and sets t and stepID
    HDF5LoadState(base_path, stateID, load_registry, P.N, timestepper.t);

    // Initialise domains, solvers, forces
    DomainManager DM(SimParams, P);
    SolverManager SM(DM);
    Forces F(SimParams, DM, SM);

    // Initialise statistics, file to be written and create registry that points to statistics buffers
    Statistics stats(SimParams); 
    HDF5StatisticsExport stat_file(base_path + "_stats.h5");
    DataRegistry stat_export_registry; RegisterStatistics(stat_export_registry, stats);


    // ------------- SIMULATION LOOP -------------

    // Synchronise acceleration with the other state data
    F.Acceleration(P);

    while (timestepper.t < timestepper.t_end) {
        // ------ COLLECT STATISTICS ------
        stats.SetStats(P, F, timestepper);

        // ------ INTEGRATE ------

        // Determine timestep
        timestepper.TimesteppingCriteria(P, F);

        // Integrate equations of motion
        integrator.Step(P, F, timestepper);

        // Advance clock
        timestepper.AdvanceClock();

        // ------ CAPTURE SIMULATION SNAPSHOT ------
        if (timestepper.step_ID % stats.snapshotInterval == 0) {
            HDF5SaveState(base_path, load_registry, P.N, timestepper.step_ID, timestepper.t);
            stat_file.Export(stat_export_registry);
            std::cout << "Saved in timestep " << timestepper.t << std::endl;
        }
    }
    
}