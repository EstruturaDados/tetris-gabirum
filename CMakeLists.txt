cmake_minimum_required(VERSION 3.30.0)
project(tetris VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

file(GLOB SOURCE_FILES "*.c")
add_executable(tetris ${SOURCE_FILES})
