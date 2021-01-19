cmake_minimum_required (VERSION 3.10)
project(bourne)

file(GLOB bourne_headers ./src/bourne/*.hpp)
file(GLOB_RECURSE bourne_sources ./src/*.cpp)

add_library(bourne STATIC ${bourne_sources})
add_library(steinwurf::bourne ALIAS bourne)

target_include_directories(bourne INTERFACE src/)

target_compile_features(bourne PUBLIC cxx_std_11)

install(FILES ${bourne_headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/bourne)
install(FILES $<TARGET_FILE:bourne> DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
