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

add_library(aether_stream src/version.cpp src/core/status.cpp)

add_library(aether::stream ALIAS aether_stream)

target_include_directories(aether_stream PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(aether_stream PUBLIC cxx_std_20)

aether_set_project_warnings(aether_stream)

if(AETHER_BUILD_EXAMPLES)
  add_executable(aether_smoke examples/smoke.cpp)
  target_link_libraries(aether_smoke PRIVATE aether::stream)
  aether_set_project_warnings(aether_smoke)
  set_target_properties(
    aether_smoke
    PROPERTIES OUTPUT_NAME smoke RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
  )
endif()

if(AETHER_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(AETHER_BUILD_BENCHMARKS)
  message(STATUS "AETHER_BUILD_BENCHMARKS is ON, but benchmarks are planned for Phase 5.")
endif()

message(STATUS "Aether-Stream Phase 2 build configured.")
