function(aether_add_test target source test_name)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE aether::stream)
  aether_set_project_warnings(${target})
  aether_enable_sanitizers(${target})
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
aether_add_test(aether_test_spsc_concurrent test_spsc_concurrent.cpp aether.spsc.concurrent)
aether_add_test(aether_test_spsc_move_only test_spsc_move_only.cpp aether.spsc.move_only)
aether_add_test(aether_test_spsc_stress test_spsc_stress.cpp aether.spsc.stress)

aether_add_test(aether_test_mmap_file test_mmap_file.cpp aether.mmap_file)

aether_add_test(aether_test_wal_record test_wal_record.cpp aether.wal.record)
aether_add_test(aether_test_wal_writer test_wal_writer.cpp aether.wal.writer)
aether_add_test(aether_test_wal_reader test_wal_reader.cpp aether.wal.reader)

aether_add_test(aether_test_counters test_counters.cpp aether.metrics.counters)
aether_add_test(aether_test_latency_histogram test_latency_histogram.cpp aether.metrics.latency_histogram)

aether_add_test(aether_test_broker test_broker.cpp aether.broker)
aether_add_test(aether_test_persistent_broker test_persistent_broker.cpp aether.persistent_broker)

aether_add_test(aether_test_cli_args test_cli_args.cpp aether.cli.args)


if(TARGET aether_app_bench)
  add_test(
    NAME aether.cli.bench_smoke
    COMMAND aether_app_bench --messages 1000 --payload-size 64 --capacity 1024
            --output-dir "${CMAKE_BINARY_DIR}/cli-smoke-results"
  )
endif()
