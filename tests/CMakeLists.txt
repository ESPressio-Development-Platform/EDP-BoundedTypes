cmake_minimum_required(VERSION 3.16)
project(EDP_BoundedTypes_Tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(
    EDP_BoundedTypes_HostTests
    host/main.cpp
)

target_include_directories(
    EDP_BoundedTypes_HostTests
    PRIVATE
    ../src
)

target_compile_options(
    EDP_BoundedTypes_HostTests
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)
