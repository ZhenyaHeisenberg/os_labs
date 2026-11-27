cmake_minimum_required(VERSION 3.10)
project(lab_5 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# будем собирать динамические библиотеки
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


add_library(sin_rect SHARED sin_rect.c)
target_link_libraries(sin_rect m)

add_library(sin_trap SHARED sin_trap.c)
target_link_libraries(sin_trap m)

add_library(prime_naive SHARED prime_naive.c)
add_library(prime_eratosthenes SHARED prime_eratosthenes.c)


add_executable(prog1_v1 prog1.c)
target_link_libraries(prog1_v1 sin_rect prime_naive)

add_executable(prog1_v2 prog1.c)
target_link_libraries(prog1_v2 sin_trap prime_eratosthenes)


add_executable(prog2 prog2.c)
