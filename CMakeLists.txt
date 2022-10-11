cmake_minimum_required(VERSION 3.0.0)

project(
    lab3
    VERSION 1.0
    DESCRIPTION "C++ Year 2022 Lab3"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wuninitialized -Wno-unused-result -Wshadow")

add_subdirectory(bin)
add_subdirectory(lib)
