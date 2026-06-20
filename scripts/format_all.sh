#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

if ! command -v clang-format >/dev/null 2>&1; then
  echo "error: clang-format not found"
  echo "Install it with Homebrew on macOS:"
  echo "  brew install llvm"
  exit 1
fi

MODE="format"
if [[ "${1:-}" == "--check" ]]; then
  MODE="check"
fi

SEARCH_DIRS=()
for dir in include src tests benchmarks apps examples tools; do
  if [[ -d "${REPO_ROOT}/${dir}" ]]; then
    SEARCH_DIRS+=("${REPO_ROOT}/${dir}")
  fi
done

if [[ ${#SEARCH_DIRS[@]} -eq 0 ]]; then
  echo "No source directories found."
  exit 0
fi

FILES=()
while IFS= read -r file; do
  FILES+=("${file}")
done < <(
  find "${SEARCH_DIRS[@]}" \
    \( -name '*.hpp' -o -name '*.cpp' -o -name '*.h' -o -name '*.c' \) \
    -type f | sort
)

if [[ ${#FILES[@]} -eq 0 ]]; then
  echo "No C/C++ files found."
  exit 0
fi

if [[ "${MODE}" == "check" ]]; then
  echo "Checking formatting for ${#FILES[@]} file(s)"
  clang-format --dry-run --Werror "${FILES[@]}"
else
  echo "Formatting ${#FILES[@]} file(s)"
  clang-format -i "${FILES[@]}"
fi

echo "clang-format completed"
