cmake_minimum_required (VERSION 3.12)
project(bourne)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
  list(APPEND cxxflags -O2 -ftree-vectorize -finline-functions)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang")
  list(APPEND cxxflags -O2)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
  list(APPEND cxxflags /O2 /MT /EHs /bigobj)
endif()

file( GLOB_RECURSE bourne_sources ./src/*.cpp)

add_library(bourne STATIC ${bourne_sources})

target_include_directories(bourne INTERFACE src/)

target_compile_options(bourne PRIVATE ${cxxflags})

target_compile_features(bourne PUBLIC cxx_std_11)
