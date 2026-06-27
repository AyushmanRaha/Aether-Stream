#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build/debug"

GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GENERATOR_ARGS=(-G Ninja)
fi

echo "==> Configuring Aether-Stream debug build"
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
  "${GENERATOR_ARGS[@]}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_BUILD_BENCHMARKS=OFF

echo "==> Building"
cmake --build "${BUILD_DIR}"

echo "==> Running tests"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

echo "==> Test run completed successfully"
