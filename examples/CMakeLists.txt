cmake_minimum_required(VERSION 3.20)
project(consumer_example)

set(CMAKE_CXX_STANDARD 23)

# Find NGE from a standard install prefix; set CMAKE_PREFIX_PATH when configuring this project.
find_package(NGE CONFIG REQUIRED)

add_executable(consumer_example main.cpp)

# Link NGE::NGE; SDL and friends are transitively linked by the imported target
target_link_libraries(consumer_example PRIVATE NGE::NGE)

# Install executable
install(TARGETS consumer_example
    RUNTIME DESTINATION bin
)

# Install assets directory (copy contents of examples/assets/)
set(EXAMPLE_ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
if(EXISTS "${EXAMPLE_ASSETS_DIR}")
    install(DIRECTORY "${EXAMPLE_ASSETS_DIR}/"
        DESTINATION "share/${PROJECT_NAME}/assets"
        FILES_MATCHING PATTERN "*"
    )
endif()
