cmake_minimum_required(VERSION 3.20)

project(
  AetherStream
  VERSION 0.1.0
  DESCRIPTION "C++20 ultra-low-latency lock-free message broker project"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AetherOptions)
include(AetherCompilerWarnings)
include(AetherDependencies)

add_library(
  aether_stream
  src/version.cpp
  src/core/status.cpp
  src/io/mmap_file.cpp
  src/broker.cpp
  src/wal/checksum.cpp
  src/wal/wal_writer.cpp
  src/wal/wal_reader.cpp
)

add_library(aether::stream ALIAS aether_stream)

target_include_directories(aether_stream PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(aether_stream PUBLIC cxx_std_20)
target_link_libraries(aether_stream PUBLIC Threads::Threads)

aether_set_project_warnings(aether_stream)

if(AETHER_BUILD_EXAMPLES)
  add_executable(aether_smoke examples/smoke.cpp)
  target_link_libraries(aether_smoke PRIVATE aether::stream)
  aether_set_project_warnings(aether_smoke)
  set_target_properties(
    aether_smoke
    PROPERTIES OUTPUT_NAME smoke RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )

  add_executable(aether_basic_spsc examples/basic_spsc.cpp)
  target_link_libraries(aether_basic_spsc PRIVATE aether::stream)
  aether_set_project_warnings(aether_basic_spsc)
  set_target_properties(
    aether_basic_spsc
    PROPERTIES OUTPUT_NAME basic_spsc RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )

  add_executable(aether_mmap_smoke examples/mmap_smoke.cpp)
  target_link_libraries(aether_mmap_smoke PRIVATE aether::stream)
  aether_set_project_warnings(aether_mmap_smoke)
  set_target_properties(
    aether_mmap_smoke
    PROPERTIES OUTPUT_NAME mmap_smoke RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )

  add_executable(aether_wal_replay examples/wal_replay.cpp)
  target_link_libraries(aether_wal_replay PRIVATE aether::stream)
  aether_set_project_warnings(aether_wal_replay)
  set_target_properties(
    aether_wal_replay
    PROPERTIES OUTPUT_NAME wal_replay RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )

  add_executable(aether_broker_basic examples/broker_basic.cpp)
  target_link_libraries(aether_broker_basic PRIVATE aether::stream)
  aether_set_project_warnings(aether_broker_basic)
  set_target_properties(
    aether_broker_basic
    PROPERTIES OUTPUT_NAME broker_basic RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )

  add_executable(aether_persistent_broker examples/persistent_broker.cpp)
  target_link_libraries(aether_persistent_broker PRIVATE aether::stream)
  aether_set_project_warnings(aether_persistent_broker)
  set_target_properties(
    aether_persistent_broker
    PROPERTIES OUTPUT_NAME persistent_broker RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )
endif()

if(AETHER_BUILD_TOOLS)
  add_executable(aether_stress_spsc tools/stress_spsc.cpp)
  target_link_libraries(aether_stress_spsc PRIVATE aether::stream)
  aether_set_project_warnings(aether_stress_spsc)
  set_target_properties(
    aether_stress_spsc
    PROPERTIES OUTPUT_NAME stress_spsc RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
  )
endif()

if(AETHER_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(AETHER_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

message(STATUS "Aether-Stream Phase 8 build configured.")
