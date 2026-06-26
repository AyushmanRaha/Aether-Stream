#!/usr/bin/env bash
set -euo pipefail

print_version() {
  local tool="$1"
  local version_arg="${2:---version}"

  if command -v "${tool}" >/dev/null 2>&1; then
    printf '\n[%s]\n' "${tool}"
    "${tool}" "${version_arg}" 2>&1 | head -n 3
  fi
}

if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "This bootstrap script is intended for macOS."
  echo "Detected: $(uname -s)"
  exit 1
fi

if ! command -v brew >/dev/null 2>&1; then
  echo "Homebrew is required for the recommended macOS development setup."
  echo "Install Homebrew from https://brew.sh/, then rerun this script."
  exit 1
fi

missing=()
for tool in git cmake ninja; do
  if ! command -v "${tool}" >/dev/null 2>&1; then
    missing+=("${tool}")
  fi
done

if ! command -v clang++ >/dev/null 2>&1 && ! command -v c++ >/dev/null 2>&1; then
  missing+=("clang++ or c++")
fi

if (( ${#missing[@]} > 0 )); then
  echo "Missing required development tools: ${missing[*]}"
  echo "Install the recommended packages with:"
  echo "  brew install cmake ninja llvm git"
  exit 1
fi

print_version brew --version
print_version git --version
print_version cmake --version
print_version ninja --version
if command -v clang++ >/dev/null 2>&1; then
  print_version clang++ --version
else
  print_version c++ --version
fi

mkdir -p build/debug build/release

echo
echo "Created build/debug and build/release directories."
echo
echo "Common local build/test commands:"
echo "  cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug"
echo "  cmake --build build/debug"
echo "  ctest --test-dir build/debug --output-on-failure"
echo
echo "Aether-Stream macOS bootstrap checks completed successfully."
