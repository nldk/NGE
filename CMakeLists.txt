cmake_minimum_required(VERSION 3.20)
project(NGE)

set(CMAKE_CXX_STANDARD 26)

# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Include directories for our engine headers
include_directories(
        engine
)

# Build NGE library
add_library(NGE STATIC
        engine/renderer.cpp
        engine/renderer.h
        engine/error.h
        engine/color.cpp
        engine/color.h
        engine/sprite.cpp
        engine/sprite.h
        engine/vec2.h
        engine/level.cpp
        engine/level.h
        engine/eventHandeling.cpp
        engine/eventHandeling.h
        engine/json.hpp
        engine/sound.cpp
        engine/sound.h
)

# Link SDL2 and SDL2_image to NGE as PUBLIC so dependents inherit them
# Prefer imported targets when available; they also propagate include dirs correctly.
target_link_libraries(NGE
        PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
)

# Build executable
add_executable(testGame testGame/main.cpp)

# Link executable to NGE (inherits SDL deps transitively)
target_link_libraries(testGame
        PRIVATE
        NGE
)
