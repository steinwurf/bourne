cmake_minimum_required(VERSION 3.10)
project(bourne)

# Define library
file(GLOB_RECURSE bourne_sources ./src/*.cpp)
add_library(bourne_internal OBJECT ${bourne_sources})
target_compile_features(bourne_internal PUBLIC cxx_std_14)

# Add interface library encapsulating the internal the library object
add_library(bourne_objects INTERFACE)
target_sources(bourne_objects INTERFACE $<TARGET_OBJECTS:bourne_internal>)
target_include_directories(bourne_objects INTERFACE src)

# Install headers excluding "detail" as these are internal to the library.
install(
  DIRECTORY ./src/bourne
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING
  PATTERN *.hpp
  PATTERN ./src/bourne/detail EXCLUDE)

# Is top level project?
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  add_library(bourne_static STATIC)
  target_link_libraries(bourne_static PUBLIC bourne_objects)
  set_target_properties(bourne_static PROPERTIES OUTPUT_NAME bourne)

  install(FILES $<TARGET_FILE:bourne_static>
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

  # Build executables
  add_executable(example examples/example.cpp)
  target_link_libraries(example bourne_static)
else()
  add_library(steinwurf::bourne ALIAS bourne_objects)
endif()
