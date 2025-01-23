cmake_minimum_required(VERSION 3.12)
project(bourne)

find_package(Python COMPONENTS Interpreter)

# Use waf to resolve dependencies
if(NOT DEFINED STEINWURF_RESOLVE)
  message(STATUS "Resolving dependencies...")
  execute_process(
    COMMAND ${Python_EXECUTABLE} waf resolve ${STEINWURF_RESOLVE_OPTIONS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE STATUS)

  if(STATUS AND NOT STATUS EQUAL 0)
    message(FATAL_ERROR "Failed: ${STATUS}")
  endif()

  set(STEINWURF_RESOLVE "${CMAKE_CURRENT_SOURCE_DIR}/resolve_symlinks")
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

  enable_testing()

  if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    # For Windows: Prevent overriding the parent project's compiler/linker
    # settings
    set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  endif()

  # Google Test dependency
  add_subdirectory("${STEINWURF_RESOLVE}/gtest-source")

  # Build test executable
  file(GLOB_RECURSE bourne_test_sources ./test/*.cpp)
  add_executable(bourne_test ${bourne_test_sources})
  target_link_libraries(bourne_test gtest_main)
  target_link_libraries(bourne_test steinwurf::bourne)

  # Copy test/test.json to build directory
  file(COPY test/test.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  add_test(bourne_test bourne_test)
endif()
