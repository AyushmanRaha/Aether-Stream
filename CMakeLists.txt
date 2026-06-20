cmake_minimum_required(VERSION 3.20)

project(
  AetherStream
  VERSION 0.1.0
  DESCRIPTION "Planned C++20 ultra-low-latency lock-free message broker"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Aether-Stream Phase 0 baseline configured.")
message(STATUS "Real library targets will be added in Phase 1.")
