function(aether_add_benchmark target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE aether::stream benchmark::benchmark Threads::Threads)
  aether_set_project_warnings(${target})
  set_target_properties(
    ${target}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/benchmarks"
  )
endfunction()

aether_add_benchmark(aether_bench_spsc_throughput bench_spsc_throughput.cpp)
set_target_properties(aether_bench_spsc_throughput PROPERTIES OUTPUT_NAME bench_spsc_throughput)

aether_add_benchmark(aether_bench_spsc_latency bench_spsc_latency.cpp)
set_target_properties(aether_bench_spsc_latency PROPERTIES OUTPUT_NAME bench_spsc_latency)

aether_add_benchmark(aether_bench_payload_sizes bench_payload_sizes.cpp)
set_target_properties(aether_bench_payload_sizes PROPERTIES OUTPUT_NAME bench_payload_sizes)
