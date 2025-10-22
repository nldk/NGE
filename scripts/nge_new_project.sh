#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: nge_new_project.sh <project-name> <destination-dir> [--with-assets] [--copy-sdk] [--sdk-prefix <path>]

Generates a minimal CMake project that links against the installed NGE package.
Optionally vendors the NGE headers and library into the generated project.

Sources for vendoring (auto-detected in this order):
  1) Global install prefix (~/.local/NGE) if it exists
  2) Local repo install at cmake-build-release/install
  3) Local repo sources: release-headers/ + cmake-build-release/libNGE.a

Options:
  --with-assets          Create an assets/ folder and add CMake install rules to copy it.
  --copy-sdk             Copy NGE headers and lib into third_party/NGE/ of the project (auto-enabled if sources found).
  --sdk-prefix <path>    Force copying from an install prefix. Overrides auto-detection.

Build instructions for the generated project:
  1) If not using --copy-sdk: Ensure NGE is installed to a prefix.
     Example (from NGE root):
       cmake --install cmake-build-release --prefix "$PWD/cmake-build-release/install"
       or run scripts/nge_install_sdk.sh (installs to ~/.local/NGE by default)
  2) Configure the consumer:
     - Vendored (if SDK was copied):
         cmake -S . -B build
     - Using installed package (no --copy-sdk):
         cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/NGE/install"  # e.g. ~/.local/NGE
  3) Build:
       cmake --build build -j
  4) Optionally install the consumer to a prefix:
       cmake --install build --prefix ./install
USAGE
}

if [[ ${1:-} == "-h" || ${1:-} == "--help" || $# -lt 2 ]]; then
  usage
  exit 0
fi

PROJECT_NAME="$1"
DEST_DIR="$2"
WITH_ASSETS=0
COPY_SDK=0
SDK_PREFIX=""

# Parse optional flags (positional after the first two args)
shift 2 || true
while (( "$#" )); do
  case "${1}" in
    --with-assets)
      WITH_ASSETS=1
      shift
      ;;
    --copy-sdk)
      COPY_SDK=1
      shift
      ;;
    --sdk-prefix)
      SDK_PREFIX="${2:-}"
      if [[ -z "$SDK_PREFIX" ]]; then
        echo "--sdk-prefix requires a path argument" >&2
        exit 2
      fi
      shift 2
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 2
      ;;
  esac
done

# Determine repo-relative paths
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
SDK_PREFIX_GLOBAL="$HOME/.local/NGE"
SDK_PREFIX_LOCAL_INSTALL="${REPO_ROOT}/cmake-build-release/install"
RELEASE_HEADERS_DIR="${REPO_ROOT}/release-headers"
BUILD_LIB_RELEASE="${REPO_ROOT}/cmake-build-release/libNGE.a"
ALT_BUILD_LIB_RELEASE="${REPO_ROOT}/cmake-build-release/NGE.a"

# Decide if we can auto-copy from local repo (release-headers + local build lib)
CAN_COPY_FROM_LOCAL=0
if [[ -d "$RELEASE_HEADERS_DIR" ]] && { [[ -f "$BUILD_LIB_RELEASE" ]] || [[ -f "$ALT_BUILD_LIB_RELEASE" ]]; }; then
  CAN_COPY_FROM_LOCAL=1
fi

# If user didn’t request copy explicitly, try to auto-enable it from best source.
if [[ "$COPY_SDK" -eq 0 && -z "${SDK_PREFIX}" ]]; then
  if [[ -d "$SDK_PREFIX_GLOBAL" ]]; then
    COPY_SDK=1
    SDK_PREFIX="$SDK_PREFIX_GLOBAL"
  elif [[ -d "$SDK_PREFIX_LOCAL_INSTALL" ]]; then
    COPY_SDK=1
    SDK_PREFIX="$SDK_PREFIX_LOCAL_INSTALL"
  elif [[ "$CAN_COPY_FROM_LOCAL" -eq 1 ]]; then
    COPY_SDK=1
  fi
fi

# If copying SDK and no prefix passed, prefer global install; else fallback to local install;
# else, if neither available, but local headers+lib exist, we'll copy from local repo.
if [[ "$COPY_SDK" -eq 1 && -z "$SDK_PREFIX" && "$CAN_COPY_FROM_LOCAL" -eq 0 ]]; then
  if [[ -d "$SDK_PREFIX_GLOBAL" ]]; then
    SDK_PREFIX="$SDK_PREFIX_GLOBAL"
  elif [[ -d "$SDK_PREFIX_LOCAL_INSTALL" ]]; then
    SDK_PREFIX="$SDK_PREFIX_LOCAL_INSTALL"
  else
    echo "Could not determine NGE SDK sources automatically. Use --sdk-prefix <path> or run scripts/nge_install_sdk.sh first." >&2
    exit 2
  fi
fi

# Compute root of generated project
PROJ_ROOT="${DEST_DIR%/}/${PROJECT_NAME}"
mkdir -p "$PROJ_ROOT/src"

# Optionally create assets folder
if [[ "$WITH_ASSETS" -eq 1 ]]; then
  mkdir -p "$PROJ_ROOT/assets"
  # placeholder to keep directory in VCS
  echo "" > "$PROJ_ROOT/assets/.placeholder"
fi

# Optionally vendor the NGE SDK (headers + lib)
if [[ "$COPY_SDK" -eq 1 ]]; then
  VENDOR_DIR="$PROJ_ROOT/third_party/NGE"
  mkdir -p "$VENDOR_DIR/lib" "$VENDOR_DIR/include"

  if [[ "$CAN_COPY_FROM_LOCAL" -eq 1 && -z "$SDK_PREFIX" ]]; then
    echo "Copying NGE headers from: $RELEASE_HEADERS_DIR"
    # Copy engine headers into include/NGE so includes like <NGE/renderer.h> work
    mkdir -p "$VENDOR_DIR/include/NGE"
    rsync -a --delete --include='*/' --include='*.h' --include='*.hpp' --exclude='*' "$RELEASE_HEADERS_DIR/" "$VENDOR_DIR/include/NGE/"
    # Ensure nge.h is included if it exists in repo engine/
    if [[ -f "$REPO_ROOT/engine/nge.h" ]]; then
      cp -a "$REPO_ROOT/engine/nge.h" "$VENDOR_DIR/include/NGE/"
    fi
    # Map SDL headers so typical includes resolve
    # If repo provides SDL headers under release-headers/SDL/include etc.
    if [[ -d "$RELEASE_HEADERS_DIR/SDL/include" ]]; then
      mkdir -p "$VENDOR_DIR/include/SDL2"
      rsync -a "$RELEASE_HEADERS_DIR/SDL/include/" "$VENDOR_DIR/include/SDL2/"
    fi
    if [[ -d "$RELEASE_HEADERS_DIR/SDL_image/include" ]]; then
      mkdir -p "$VENDOR_DIR/include/SDL2_image"
      rsync -a "$RELEASE_HEADERS_DIR/SDL_image/include/" "$VENDOR_DIR/include/SDL2_image/"
    fi
    if [[ -d "$RELEASE_HEADERS_DIR/SDL_mixer/include" ]]; then
      mkdir -p "$VENDOR_DIR/include/SDL2_mixer"
      rsync -a "$RELEASE_HEADERS_DIR/SDL_mixer/include/" "$VENDOR_DIR/include/SDL2_mixer/"
    fi

    echo "Copying NGE library from local build"
    if [[ -f "$BUILD_LIB_RELEASE" ]]; then
      cp -a "$BUILD_LIB_RELEASE" "$VENDOR_DIR/lib/"
    elif [[ -f "$ALT_BUILD_LIB_RELEASE" ]]; then
      # Fallback name NGE.a -> copy but also duplicate name to libNGE.a for simpler linking
      cp -a "$ALT_BUILD_LIB_RELEASE" "$VENDOR_DIR/lib/libNGE.a"
    else
      echo "Warning: No libNGE.* found in local build. Linking may fail." >&2
    fi
  else
    # Copy from install prefix (headers and libs)
    echo "Copying NGE SDK from install prefix: $SDK_PREFIX"
    if [[ ! -d "$SDK_PREFIX/include" || ! -d "$SDK_PREFIX/lib" ]]; then
      echo "SDK prefix does not look valid (missing include/ or lib/): $SDK_PREFIX" >&2
      exit 3
    fi
    rsync -a --delete "$SDK_PREFIX/include/" "$VENDOR_DIR/include/"
    shopt -s nullglob
    LIBS_TO_COPY=( "$SDK_PREFIX/lib/libNGE.a" "$SDK_PREFIX/lib/libNGE.so" "$SDK_PREFIX/lib/libNGE.dylib" )
    COPIED_ANY=0
    for lib in "${LIBS_TO_COPY[@]}"; do
      if [[ -f "$lib" ]]; then
        cp -a "$lib" "$VENDOR_DIR/lib/"
        COPIED_ANY=1
      fi
    done
    shopt -u nullglob
    if [[ "$COPIED_ANY" -eq 0 ]]; then
      echo "Warning: No libNGE.* found under $SDK_PREFIX/lib. The project will generate, but linking may fail." >&2
    fi
  fi
fi

# Generate main.cpp
cat > "$PROJ_ROOT/src/main.cpp" <<'CPP'
#include <iostream>
#include <NGE/nge.h>

int main() {
    std::cout << "Hello from generated NGE app!" << std::endl;
    return 0;
}
CPP

# Generate CMakeLists.txt
cat > "$PROJ_ROOT/CMakeLists.txt" <<'CMAKE'
cmake_minimum_required(VERSION 3.20)
project(PROJECT_NAME_PLACEHOLDER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp)

# Use vendored NGE if third_party/NGE exists and contains libNGE.a; otherwise use installed package.
set(NGE_VENDORED_ROOT "${CMAKE_SOURCE_DIR}/third_party/NGE")
set(NGE_VENDORED_LIB  "${NGE_VENDORED_ROOT}/lib/libNGE.a")
if(EXISTS "${NGE_VENDORED_LIB}")
    message(STATUS "Using vendored NGE: ${NGE_VENDORED_ROOT}")
    # SDL dependencies are still required at link time
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE "${NGE_VENDORED_ROOT}/include")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "${NGE_VENDORED_LIB}"
        SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
else()
    # Fallback: use installed NGE package (requires CMAKE_PREFIX_PATH)
    find_package(NGE CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE NGE::NGE)
endif()

# Install executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Optionally install assets if the folder exists
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
        DESTINATION "share/${PROJECT_NAME}/assets"
        FILES_MATCHING PATTERN "*")
endif()
CMAKE
# Replace placeholder with actual project name safely
sed -i "s/PROJECT_NAME_PLACEHOLDER/${PROJECT_NAME//\//\/}/g" "$PROJ_ROOT/CMakeLists.txt"

# Generate README.md with quick-start instructions
cat > "$PROJ_ROOT/README.md" <<'MD'
# Generated NGE Consumer Project

## Modes
- Vendored SDK (auto if `third_party/NGE/` exists and contains libNGE.a):
  - Uses the headers and lib copied into `third_party/NGE/`.
  - You still need SDL2, SDL2_image, SDL2_mixer installed on the system for linking/runtime.
- Installed package (if not vendored):
  - Uses CMake `find_package(NGE CONFIG REQUIRED)` with `CMAKE_PREFIX_PATH` pointing to the NGE install prefix (e.g. `~/.local/NGE`).

## Build
Vendored (if `third_party/NGE/` exists and contains libNGE.a):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Using installed NGE package:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$HOME/.local/NGE"
cmake --build build -j
```

## Run
```bash
./build/PROJECT_NAME_PLACEHOLDER
```

## Install (optional)
```bash
cmake --install build --prefix ./install
```
This will place:
- Binary at `install/bin/PROJECT_NAME_PLACEHOLDER`
- If present, assets at `install/share/PROJECT_NAME_PLACEHOLDER/assets`

## Using assets at runtime
If you installed, your asset root is typically:
```
<executable-dir>/../share/PROJECT_NAME_PLACEHOLDER/assets
```
MD
# Replace placeholders in README
sed -i "s/PROJECT_NAME_PLACEHOLDER/${PROJECT_NAME//\//\/}/g" "$PROJ_ROOT/README.md"

# Done
printf "\nCreated project at: %s\n" "$PROJ_ROOT"
if [[ "$WITH_ASSETS" -eq 1 ]]; then
  echo "(An empty assets/ folder was created and will be installed if you use \"cmake --install\".)"
fi
if [[ "$COPY_SDK" -eq 1 ]]; then
  echo "(Vendored NGE headers and library copied to third_party/NGE.)"
fi
