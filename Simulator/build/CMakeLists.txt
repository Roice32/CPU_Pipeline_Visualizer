cmake_minimum_required(VERSION 3.0.0)
project(CPU_Pipeline_Simulator VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_ROOT "../src")

file(GLOB_RECURSE
    SOURCES CONFIGURE_DEPENDS
        "${SRC_ROOT}/*.cpp"
        "${SRC_ROOT}/cpu/*.cpp"
        "${SRC_ROOT}/logging/*.cpp"
        "${SRC_ROOT}/memory/*.cpp"
        "${SRC_ROOT}/modules/load_store/*.cpp"
        "${SRC_ROOT}/modules/instruction_cache/*.cpp"
        "${SRC_ROOT}/modules/decode/*.cpp"
        "${SRC_ROOT}/modules/execute/*.cpp"
        "${SRC_ROOT}/modules/execute/execution_strategies/*.cpp")

add_executable(CPU_Pipeline_Simulator "${SRC_ROOT}/main.cpp" ${SOURCES})

target_include_directories(CPU_Pipeline_Simulator
    PUBLIC 
        "${SRC_ROOT}"
        "${SRC_ROOT}/cpu"
        "${SRC_ROOT}/logging"
        "${SRC_ROOT}/cache"
        "${SRC_ROOT}/synchronization"
        "${SRC_ROOT}/memory"
        "${SRC_ROOT}/modules/load_store"
        "${SRC_ROOT}/modules/instruction_cache"
        "${SRC_ROOT}/modules/decode"
        "${SRC_ROOT}/modules/execute"
        "${SRC_ROOT}/modules/execute/execution_strategies")

target_include_directories(CPU_Pipeline_Simulator
    PUBLIC 
        "../interfaces")

target_link_libraries(CPU_Pipeline_Simulator pthread)

