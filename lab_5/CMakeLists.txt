cmake_minimum_required(VERSION 3.10)
project(lab_4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(parent src/parent.c)
target_link_libraries(parent mman)

add_executable(child src/child.c)
target_link_libraries(child mman)