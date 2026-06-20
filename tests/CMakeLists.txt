add_executable(aether_test_version test_version.cpp)

target_link_libraries(aether_test_version PRIVATE aether::stream)

aether_set_project_warnings(aether_test_version)

set_target_properties(
  aether_test_version
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

add_test(NAME aether.version COMMAND aether_test_version)
