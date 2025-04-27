cmake_minimum_required(VERSION 3.0.0)
project(CPU_Pipeline_Visualizer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE
    SOURCES CONFIGURE_DEPENDS
        "../sources/*.cpp"
        "../sources/cpu/*.cpp"
        "../sources/logging/*.cpp"
        "../sources/modules/load_store/*.cpp"
        "../sources/modules/instruction_cache/*.cpp"
        "../sources/modules/decode/*.cpp"
        "../sources/modules/execute/*.cpp"
        "../sources/modules/execute/execution_strategies/*.cpp")

add_executable(CPU_Pipeline_Visualizer "../main.cpp" ${SOURCES})

target_include_directories(CPU_Pipeline_Visualizer
    PUBLIC 
        "../headers"
        "../headers/cpu"
        "../headers/logging"
        "../headers/cache"
        "../headers/synchronization"
        "../headers/modules/load_store"
        "../headers/modules/instruction_cache"
        "../headers/modules/decode"
        "../headers/modules/execute"
        "../headers/modules/execute/execution_strategies")

target_include_directories(CPU_Pipeline_Visualizer
    PUBLIC 
        "../interfaces")

target_link_libraries(CPU_Pipeline_Visualizer pthread)

