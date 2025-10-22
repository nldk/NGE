#!/usr/bin/env bash
set -euo pipefail

# Create a release package folder with headers, library and CMake package so
# people can link against the shipped .a and include files.
#
# Usage:
#   scripts/make_release_package.sh [--out <path>] [--from-prefix <install-prefix>] [--include-assets] [--include-examples]
#
# Defaults:
#   out: <repo-root>/release
#   from-prefix: prefer <repo-root>/cmake-build-release/install then ~/.local/NGE if available
#
# Result layout (under <out>):
#   include/NGE/...       # public headers
#   lib/libNGE.a          # static archive
#   lib/cmake/NGE/...      # exported CMake package files
#   share/NGE/assets/...   # optional assets
#   bin/testGame          # optional example binary

OUT=""
FROM_PREFIX=""
INCLUDE_ASSETS=0
INCLUDE_EXAMPLES=0

while (( "$#" )); do
  case "$1" in
    --out)
      OUT="${2:-}"
      shift 2
      ;;
    --from-prefix)
      FROM_PREFIX="${2:-}"
      shift 2
      ;;
    --include-assets)
      INCLUDE_ASSETS=1
      shift
      ;;
    --include-examples)
      INCLUDE_EXAMPLES=1
      shift
      ;;
    -h|--help)
      sed -n '1,200p' "$0"
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

: ${OUT:="${REPO_ROOT}/release"}

# Candidate install prefixes to copy from (preferred order)
LOCAL_INSTALL="${REPO_ROOT}/cmake-build-release/install"
GLOBAL_INSTALL="$HOME/.local/NGE"

if [[ -n "$FROM_PREFIX" ]]; then
  PREFIX="$FROM_PREFIX"
elif [[ -d "$LOCAL_INSTALL" ]]; then
  PREFIX="$LOCAL_INSTALL"
elif [[ -d "$GLOBAL_INSTALL" ]]; then
  PREFIX="$GLOBAL_INSTALL"
else
  PREFIX=""
fi

mkdir -p "$OUT"

# Helper to copy files with rsync and preserve dirs
copy_rsync() {
  local src="$1" dst="$2"
  mkdir -p "$dst"
  rsync -a --delete "$src" "$dst"
}

if [[ -n "$PREFIX" && -d "$PREFIX" ]]; then
  echo "Using install prefix: $PREFIX"
  # copy include/ -> OUT/include
  if [[ -d "$PREFIX/include" ]]; then
    echo "Copying headers from $PREFIX/include -> $OUT/include"
    copy_rsync "$PREFIX/include/" "$OUT/include/"
  fi
  # copy lib
  if [[ -d "$PREFIX/lib" ]]; then
    mkdir -p "$OUT/lib"
    shopt -s nullglob
    cp -a $PREFIX/lib/libNGE.* "$OUT/lib/" 2>/dev/null || true
    shopt -u nullglob
    # copy cmake package files if present
    if [[ -d "$PREFIX/lib/cmake/NGE" ]]; then
      mkdir -p "$OUT/lib/cmake/NGE"
      copy_rsync "$PREFIX/lib/cmake/NGE/" "$OUT/lib/cmake/NGE/"
    fi
  fi
else
  echo "No install prefix found. Falling back to repo sources."
  # Headers: use release-headers/
  if [[ -d "${REPO_ROOT}/release-headers" ]]; then
    echo "Copying headers from release-headers -> $OUT/include/NGE"
    mkdir -p "$OUT/include/NGE"
    rsync -a --include='*/' --include='*.h' --include='*.hpp' --exclude='*' "${REPO_ROOT}/release-headers/" "$OUT/include/NGE/"
    # copy nge.h from engine if present
    if [[ -f "${REPO_ROOT}/engine/nge.h" ]]; then
      cp -a "${REPO_ROOT}/engine/nge.h" "$OUT/include/NGE/"
    fi
  fi
  # Library: try to find built archives in common places
  CANDIDATES=("${REPO_ROOT}/cmake-build-release/libNGE.a" "${REPO_ROOT}/cmake-build-release/NGE.a" "${REPO_ROOT}/cmake-build-release/lib/libNGE.a" "${REPO_ROOT}/libNGE.a" "${REPO_ROOT}/lib/libNGE.a")
  mkdir -p "$OUT/lib"
  FOUND_LIB=0
  for c in "${CANDIDATES[@]}"; do
    if [[ -f "$c" ]]; then
      echo "Copying library $c -> $OUT/lib/"
      cp -a "$c" "$OUT/lib/"
      FOUND_LIB=1
      break
    fi
  done
  if [[ "$FOUND_LIB" -eq 0 ]]; then
    echo "Warning: no libNGE.* found in build tree. The release folder will be incomplete." >&2
  fi
  # CMake package is not available when not installed; consumer should use provided include+lib.
fi

# Optionally include assets
if [[ "$INCLUDE_ASSETS" -eq 1 ]]; then
  # Prefer assets in repo root or testGame folder
  if [[ -d "${REPO_ROOT}/assets" ]]; then
    echo "Copying repo assets -> $OUT/share/NGE/assets"
    mkdir -p "$OUT/share/NGE/assets"
    rsync -a --delete "${REPO_ROOT}/assets/" "$OUT/share/NGE/assets/"
  elif [[ -d "${REPO_ROOT}/testGame" ]]; then
    if [[ -d "${REPO_ROOT}/testGame/textures" || -d "${REPO_ROOT}/testGame/sound" || -d "${REPO_ROOT}/testGame/levels" ]]; then
      echo "Copying testGame assets -> $OUT/share/NGE/assets"
      mkdir -p "$OUT/share/NGE/assets"
      rsync -a --delete "${REPO_ROOT}/testGame/textures/" "$OUT/share/NGE/assets/textures/" || true
      rsync -a --delete "${REPO_ROOT}/testGame/sound/" "$OUT/share/NGE/assets/sound/" || true
      rsync -a --delete "${REPO_ROOT}/testGame/levels/" "$OUT/share/NGE/assets/levels/" || true
    fi
  fi
fi

# Optionally include example binaries
if [[ "$INCLUDE_EXAMPLES" -eq 1 ]]; then
  # try installed bin first
  if [[ -n "$PREFIX" && -x "$PREFIX/bin/testGame" ]]; then
    echo "Copying testGame binary from $PREFIX/bin -> $OUT/bin"
    mkdir -p "$OUT/bin"
    cp -a "$PREFIX/bin/testGame" "$OUT/bin/"
  elif [[ -x "${REPO_ROOT}/cmake-build-release/testGame" ]]; then
    mkdir -p "$OUT/bin"
    cp -a "${REPO_ROOT}/cmake-build-release/testGame" "$OUT/bin/"
  elif [[ -x "${REPO_ROOT}/testGame/testGame" ]]; then
    mkdir -p "$OUT/bin"
    cp -a "${REPO_ROOT}/testGame/testGame" "$OUT/bin/"
  else
    echo "No testGame binary found to include." >&2
  fi
fi

# Final summary
echo "\nRelease package created at: $OUT"
find "$OUT" -maxdepth 3 -print | sed 's|^|  |'

exit 0

