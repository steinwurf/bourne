cmake_minimum_required (VERSION 3.10)
project(bourne)

file(GLOB_RECURSE bourne_sources ./src/*.cpp)

add_library(bourne STATIC ${bourne_sources})
add_library(steinwurf::bourne ALIAS bourne)

target_include_directories(bourne INTERFACE src/)

target_compile_features(bourne PUBLIC cxx_std_11)
