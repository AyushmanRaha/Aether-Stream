function(aether_add_test target source test_name)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE aether::stream)
  aether_set_project_warnings(${target})
  set_target_properties(
    ${target}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
  )
  add_test(NAME ${test_name} COMMAND ${target})
endfunction()

aether_add_test(aether_test_version test_version.cpp aether.version)
aether_add_test(aether_test_status test_status.cpp aether.status)
aether_add_test(aether_test_message test_message.cpp aether.message)
aether_add_test(aether_test_spsc_basic test_spsc_basic.cpp aether.spsc.basic)
aether_add_test(aether_test_spsc_wraparound test_spsc_wraparound.cpp aether.spsc.wraparound)
