#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: nge_new_project.sh <project-name> <destination-dir> [--with-assets]

Generates a minimal CMake project that links against the installed NGE package.

Options:
  --with-assets    Create an assets/ folder and add CMake install rules to copy it.

Build instructions for the generated project:
  1) Ensure NGE is installed or built with an install prefix.
     Example (from NGE root):
       cmake --install cmake-build-debug --prefix "$PWD/cmake-build-debug/install"
  2) Configure the consumer with CMAKE_PREFIX_PATH pointing to the NGE install prefix:
       cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/NGE/install"
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

if [[ ${3:-} == "--with-assets" ]]; then
  WITH_ASSETS=1
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

# Generate main.cpp
cat > "$PROJ_ROOT/src/main.cpp" <<'CPP'
#include <iostream>
#include <NGE/nge.h>

int main() {
    std::cout << "Hello from generated NGE app!" << std::endl;
    // If your engine requires initialization, do it here.
    // For now, this just ensures headers and linking are correct.
    return 0;
}
CPP

# Generate CMakeLists.txt
cat > "$PROJ_ROOT/CMakeLists.txt" <<CMAKE
cmake_minimum_required(VERSION 3.20)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the installed NGE package. You must set CMAKE_PREFIX_PATH to the NGE install prefix.
# Example: cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/NGE/install"
find_package(NGE CONFIG REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE NGE::NGE)

# Install executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Optionally install assets if the folder exists
if(EXISTS "\${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY "\${CMAKE_SOURCE_DIR}/assets/"
        DESTINATION "share/\${PROJECT_NAME}/assets"
        FILES_MATCHING PATTERN "*")
endif()
CMAKE

# Generate README.md with quick-start instructions
cat > "$PROJ_ROOT/README.md" <<'MD'
# Generated NGE Consumer Project

## Prerequisites
- NGE installed to a prefix containing `include/`, `lib/`, and `lib/cmake/NGE/`.
- SDL2, SDL2_image, SDL2_mixer libraries installed on your system (as required by NGE).

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/path/to/NGE/install"
cmake --build build -j
```

## Run
```bash
./build/${PROJECT_NAME}
```

## Install (optional)
```bash
cmake --install build --prefix ./install
```
This will place:
- Binary at `install/bin/${PROJECT_NAME}`
- If present, assets at `install/share/${PROJECT_NAME}/assets`

## Using assets at runtime
If you installed, your asset root is typically:
```
<executable-dir>/../share/${PROJECT_NAME}/assets
```
MD

# Done
printf "\nCreated project at: %s\n" "$PROJ_ROOT"
if [[ "$WITH_ASSETS" -eq 1 ]]; then
  echo "(An empty assets/ folder was created and will be installed if you use \"cmake --install\".)"
fi

