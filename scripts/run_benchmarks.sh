#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${AETHER_BUILD_DIR:-${REPO_ROOT}/build/release}"
RESULT_ROOT="${REPO_ROOT}/benchmark-results"
RESULT_DIR="${RESULT_ROOT}/$(date +%Y%m%d-%H%M%S)"
CXX_BIN="${CXX:-c++}"

GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GENERATOR_ARGS=(-G Ninja)
fi

BENCHMARK_ARGS=(--benchmark_min_time=2s)
if (($# > 0)); then
  BENCHMARK_ARGS+=("$@")
fi

mkdir -p "${RESULT_DIR}"

write_environment() {
  {
    echo "# Aether-Stream benchmark environment"
    echo
    echo "Result directory: ${RESULT_DIR}"
    echo "Timestamp UTC: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo

    if command -v git >/dev/null 2>&1; then
      echo "## Git"
      git -C "${REPO_ROOT}" rev-parse HEAD || true
      echo
      git -C "${REPO_ROOT}" status --short || true
      echo
    fi

    echo "## System"
    uname -a || true
    echo

    if command -v sw_vers >/dev/null 2>&1; then
      echo "## macOS"
      sw_vers || true
      echo
    fi

    if command -v lsb_release >/dev/null 2>&1; then
      echo "## Linux distribution"
      lsb_release -a || true
      echo
    fi

    if command -v sysctl >/dev/null 2>&1; then
      echo "## sysctl CPU/RAM"
      sysctl machdep.cpu.brand_string 2>/dev/null || true
      sysctl hw.memsize 2>/dev/null || true
      sysctl hw.ncpu 2>/dev/null || true
      echo
    fi

    if command -v nproc >/dev/null 2>&1; then
      echo "nproc: $(nproc)"
      echo
    fi

    echo "## CMake"
    cmake --version || true
    echo

    echo "## C++ compiler"
    "${CXX_BIN}" --version || true
  } >"${RESULT_DIR}/environment.txt"
}

run_benchmark() {
  local name="$1"
  local executable="${BUILD_DIR}/benchmarks/${name}"
  local text_output="${RESULT_DIR}/${name}.txt"
  local json_output="${RESULT_DIR}/${name}.json"

  echo "==> Running ${name}"
  "${executable}" \
    "${BENCHMARK_ARGS[@]}" \
    --benchmark_out_format=json \
    --benchmark_out="${json_output}" \
    2>&1 | tee "${text_output}"
}

echo "==> Configuring Aether-Stream release benchmark build"
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
  "${GENERATOR_ARGS[@]}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_BENCHMARKS=ON

echo "==> Building"
cmake --build "${BUILD_DIR}"

echo "==> Running tests before benchmarks"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

write_environment

run_benchmark bench_spsc_throughput
run_benchmark bench_spsc_latency
run_benchmark bench_payload_sizes

echo "==> Benchmark raw outputs written to: ${RESULT_DIR}"
