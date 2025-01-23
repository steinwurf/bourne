cmake_minimum_required(VERSION 3.12)
project(bourne)

if(NOT DEFINED STEINWURF_TOP_NAME)
  set(STEINWURF_TOP_NAME bourne)
endif()

# Define library
file(GLOB_RECURSE bourne_sources ./src/*.cpp)

# Is this the top-level steinwurf project?
if(${PROJECT_NAME} STREQUAL ${STEINWURF_TOP_NAME})
  # Create static library
  add_library(bourne STATIC ${bourne_sources})

  # Install library
  install(FILES $<TARGET_FILE:bourne> DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  # Create object library
  add_library(bourne OBJECT ${bourne_sources})

  # Add this library to a global list of steinwurf object libraries
  set_property(GLOBAL APPEND PROPERTY steinwurf::object_libraries
    steinwurf::bourne)
endif()

target_include_directories(bourne INTERFACE src)
target_compile_features(bourne PUBLIC cxx_std_17)
add_library(steinwurf::bourne ALIAS bourne)

# Install headers excluding "detail" as these are internal to the library.
install(
  DIRECTORY ./src/bourne
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN *.hpp
  PATTERN ./src/bourne/detail EXCLUDE)

# Is top level project?
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  # Build executables
  add_executable(example examples/example.cpp)
  target_link_libraries(example bourne)
endif()
