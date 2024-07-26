cmake_minimum_required(VERSION 3.0.0)
project(cpp_processor_model VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_executable(cpp_processor_model "../main.cpp" ${SOURCES})

target_include_directories(cpp_processor_model
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

target_include_directories(cpp_processor_model
    PUBLIC 
        "../interfaces")

