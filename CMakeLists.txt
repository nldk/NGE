cmake_minimum_required(VERSION 3.20)
project(NGE)

set(CMAKE_CXX_STANDARD 26)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(NGE STATIC
        engine/renderer.cpp
        engine/renderer.h
        engine/error.h
        engine/color.cpp
        engine/color.h
        engine/sprite.cpp
        engine/sprite.h
        engine/vec2.h
)

add_executable(testGame testGame/main.cpp)
target_link_libraries(testGame NGE ${SDL2_LIBRARIES})