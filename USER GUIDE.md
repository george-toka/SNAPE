# 🌌 NBody Engine User Manual

**Building, Managing, and Executing Multiple Engine Builds**

---

### ⚙️ 1. Compile-Time Configuration & Automated Build

You no longer need to manually initialize CMake. Navigate to your global configuration file and set the `"value"` fields under the `COMPILE_TIME` and `CMAKE_OPTIONS` parameters to match your desired physical models, solvers, and compiler flags. 

Once configured, run the master build script:
```bash
python build_engine.py
```
The script will prompt you for a project name. It will automatically initialize CMake and isolate all binaries, receipts, and generated files into a dedicated `build/<project_name>` folder.

### 🔄 2. Recompiling vs. Reconfiguring

Because your builds are now isolated, you have two distinct workflows for updating your engine:

*   **Changing Source Code (Fast Recompile):** If you simply edit an existing `.cpp` file, you do not need to reconfigure CMake. You can run the Python script and answer `n` when prompted to reconfigure, or run the build command directly in your terminal:
    ```bash
    cmake --build build/<project_name> -j 4
    ```
*   **Changing Architecture (Full Reconfigure):** If you add new `.cpp` files to the `src/` directory, or if you want to alter the compile-time parameters, run `python build_engine.py`, enter the existing project name, and answer `y` to reconfigure. The script will safely rebuild the cache using the local receipt.

### 📄 3. Generated Configuration Files

Upon successful compilation, the script generates critical files inside your `build/<project_name>` folder:

*   **Configuration Receipt (`build_config.json`):** An exact copy of the global configuration used to generate this specific build. It ensures you never lose track of what macros are active.
*   **Runtime Parameter File (`runtime_settings.cfg`):** Contains the physical and mathematical variables based strictly on your compile-time choices. Edit this file to define the parameters for your tests.
*   **Engine Action File:** A control script that dictates exactly what operational flow the engine should execute.

> **⚠️ Note on File Integrity:** If the left-hand side (LHS) values of the runtime parameter file are heavily tampered with or corrupted, you may need to regenerate the files by re-running `build_engine.py`.

### 🕹️ 4. Engine Action & Execution

Inside the engine action file, you must define the execution behavior. 

**Select your Engine Mode:**
1.  **ICGen:** Generate Initial Conditions (ICs) only.
2.  **SimStandard:** Run a simulation using the most recent runtime parameters file.
3.  **SimNewParams:** Run a simulation using a newly defined runtime parameters file.
4.  **ICGenNRun:** Generate new Initial Conditions and immediately begin simulating.

**Define your Target State & Output Destination:**
In addition to the run mode, specify the exact scenario variables so the engine outputs organized, easy-to-read data:
*   **Naming Convention:** The file prefix used for exported snapshot data.
*   **StateID:** The exact step or snapshot ID used to start or resume a simulation from a specific point in time.
*   **Test Folder:** The specific directory the engine will operate within.

> **🛑 CRITICAL: Manual Directory Creation Required**
> The engine **does not** create output directories. You must manually create the root `tests/` folder and your specific subfolder (e.g., `tests/plummer_run/`) before running the execution script. If the engine attempts to save a state to a folder that does not exist, it will instantly fail or crash.