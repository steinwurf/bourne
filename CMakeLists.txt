cmake_minimum_required (VERSION 3.12)

file( GLOB_RECURSE bourne_sources ./src/*.cpp)

add_library(bourne STATIC ${bourne_sources})

target_include_directories(bourne INTERFACE src/)


