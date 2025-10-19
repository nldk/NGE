cmake_minimum_required(VERSION 3.20)
project(consumer_example)

set(CMAKE_CXX_STANDARD 23)

# Point find_package to the local install prefix of the NGE project
find_package(NGE CONFIG REQUIRED PATHS "${CMAKE_SOURCE_DIR}/../../cmake-build-debug/install/lib/cmake/NGE" NO_DEFAULT_PATH)

add_executable(consumer_example main.cpp)

# Link NGE::NGE; SDL and friends are transitively linked by the imported target
target_link_libraries(consumer_example PRIVATE NGE::NGE)

# Add an include so we can include <NGE/nge.h>
target_include_directories(consumer_example PRIVATE ${CMAKE_SOURCE_DIR}/../../cmake-build-debug/install/include)

