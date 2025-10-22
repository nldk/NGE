#!/usr/bin/env bash
set -euo pipefail

# Installs the NGE SDK (lib + headers + CMake package) to a prefix so
# projects and the generator can find it anywhere.
#
# Usage:
#   scripts/nge_install_sdk.sh [--prefix <path>] [--build-type <Release|Debug>] [--with-assets]
#
# Defaults:
#   prefix: ~/.local/NGE
#   build-type: Release
#
# After installation, you can:
#   - Use find_package(NGE CONFIG) with CMAKE_PREFIX_PATH pointing to <prefix>
#   - Or run scripts/nge_new_project.sh with --sdk-prefix <prefix> (auto-detected)

PREFIX_DEFAULT="$HOME/.local/NGE"
BUILD_TYPE_DEFAULT="Release"
WITH_ASSETS=0
PREFIX="$PREFIX_DEFAULT"
BUILD_TYPE="$BUILD_TYPE_DEFAULT"

while (( "$#" )); do
  case "$1" in
    --prefix)
      PREFIX="${2:-}"
      if [[ -z "$PREFIX" ]]; then
        echo "--prefix requires a path" >&2
        exit 2
      fi
      shift 2
      ;;
    --build-type)
      BUILD_TYPE="${2:-}"
      if [[ "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "Debug" ]]; then
        echo "--build-type must be Release or Debug" >&2
        exit 2
      fi
      shift 2
      ;;
    --with-assets)
      WITH_ASSETS=1
      shift
      ;;
    -h|--help)
      sed -n '1,40p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 2
      ;;
  esac
done

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/cmake-build-${BUILD_TYPE,,}"

mkdir -p "$BUILD_DIR"

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build "$BUILD_DIR" -j
cmake --install "$BUILD_DIR" --prefix "$PREFIX"

# Optionally install repo assets into the SDK prefix for convenience
if [[ "$WITH_ASSETS" -eq 1 ]]; then
  if [[ -d "${REPO_ROOT}/assets" ]]; then
    mkdir -p "$PREFIX/share/NGE/assets"
    rsync -a --delete "${REPO_ROOT}/assets/" "$PREFIX/share/NGE/assets/"
  fi
fi

echo "\nNGE SDK installed to: $PREFIX"
echo "\nTo use it in another CMake project:"
echo "  - Set CMAKE_PREFIX_PATH=\"$PREFIX\""
echo "  - Then: find_package(NGE CONFIG REQUIRED) and link NGE::NGE"

