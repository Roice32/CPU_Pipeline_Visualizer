cmake_minimum_required(VERSION 3.0.0)
project(cpp_processor_model VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "../sources/*.cpp" "../sources/modules/*.cpp" "../sources/execution_strategies/*.cpp" "../sources/synchronization/*.cpp")

add_executable(cpp_processor_model "../main.cpp" ${SOURCES})

target_include_directories(cpp_processor_model PUBLIC "../headers" "../headers/modules" "../headers/execution_strategies" "../headers/synchronization")
target_include_directories(cpp_processor_model PUBLIC "../interfaces")

