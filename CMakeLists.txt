cmake_minimum_required(VERSION 3.20)
project(NGE VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 26)

# --- Find SDL2 and related libs ---
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# --- Build the static library ---
add_library(NGE STATIC
        engine/renderer.cpp
        engine/color.cpp
        engine/sprite.cpp
        engine/level.cpp
        engine/eventHandeling.cpp
        engine/sound.cpp
        engine/button.cpp
)

# --- Include your engine and SDL headers ---
# Public include directories split for build vs install (so exported config is relocatable)
target_include_directories(NGE
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/engine>
        $<INSTALL_INTERFACE:include/NGE>
)

# If you need to expose bundled SDL headers during build (they live in the repo),
# add them only to the BUILD_INTERFACE so they won't become absolute paths in the
# installed target's INTERFACE_INCLUDE_DIRECTORIES. The installed headers will
# be available under include/NGE because we install the engine/ directory.
target_include_directories(NGE
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SDL/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SDL_image/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SDL_mixer/include>
)

# --- Link SDL2 so anything using NGE automatically inherits it ---
target_link_libraries(NGE
        PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
)

# --- Example test program ---
add_executable(testGame testGame/main.cpp)
target_link_libraries(testGame PRIVATE NGE)

# --- Install / Export rules so consumers can just link NGE::NGE ---
include(CMakePackageConfigHelpers)

# Install the static lib and export the target
install(TARGETS NGE
        EXPORT NGETargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

# Install headers (install the engine headers as public API)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/engine/
        DESTINATION include/NGE
        FILES_MATCHING PATTERN "*.h"
)

# Export the targets file
install(EXPORT NGETargets
        FILE NGETargets.cmake
        NAMESPACE NGE::
        DESTINATION lib/cmake/NGE
)

# Also export the targets into the build tree so find_package can work without
# installing the package (useful for local development / examples).
export(EXPORT NGETargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NGETargets.cmake"
       NAMESPACE NGE::
)

# Create a simple NGEConfig.cmake that will pull in the exported targets and
# ensure SDL dependencies are found when someone calls find_package(NGE CONFIG)
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NGEConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/NGEConfig.cmake"
        INSTALL_DESTINATION lib/cmake/NGE
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NGEConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NGEConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NGEConfigVersion.cmake"
        DESTINATION lib/cmake/NGE
)

# If you want to allow consumers to link just the .a and include dirs without
# using find_package, they can point to the installed include/ and lib/ paths.
