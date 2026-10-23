cmake_minimum_required(VERSION 3.15)
project(NBody_Engine LANGUAGES CXX)

# 1. Performance & Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/generated/cmake_options.cmake)

# 2. Dependency Discovery
find_package(HDF5 REQUIRED COMPONENTS C)

# 3. Recursive Discovery
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

# 4. Build the Binary
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# 5. Include Paths
target_include_directories(${PROJECT_NAME}
    PRIVATE
    src
    src/ic
    src/io
    src/integrators
    src/core
    src/forces
    src/parameters
    src/stats
    src/forces/gravity
    src/forces/hydro
    src/domain
    src/solvers
    ${CMAKE_BINARY_DIR}/generated
    ${HDF5_INCLUDE_DIRS}
)

# 6. Linking
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${HDF5_LIBRARIES}
)

# 7. Preservation of Static Macros
if(APPLE)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-all_load")
elseif(NOT MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--no-as-needed")
endif()