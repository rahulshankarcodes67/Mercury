cmake_minimum_required(VERSION 3.14)
project(Mercury VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
enable_testing()

# Engine Library
# (We will add files as we create them)
add_library(mercury_engine 
)

target_include_directories(mercury_engine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Main Executable
add_executable(mercury src/main.cpp)
target_link_libraries(mercury PRIVATE mercury_engine)
