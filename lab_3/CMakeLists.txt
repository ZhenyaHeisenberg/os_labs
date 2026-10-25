cmake_minimum_required(VERSION 3.20)
project(lab3_threads C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_path(GMP_INCLUDE_DIR gmp.h)
find_library(GMP_LIBRARY NAMES gmp libgmp)

if(NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY)
    message(FATAL_ERROR
        "GMP not found. Install it:\n"
        "  MSYS2 UCRT64: pacman -S mingw-w64-ucrt-x86_64-gmp\n"
        "  Linux:        sudo apt install libgmp-dev")
endif()

message(STATUS "GMP include: ${GMP_INCLUDE_DIR}")
message(STATUS "GMP library: ${GMP_LIBRARY}")

add_executable(lab3 src/main.c)

target_include_directories(lab3 PRIVATE ${GMP_INCLUDE_DIR})
target_link_libraries(lab3 PRIVATE ${GMP_LIBRARY})


if(WIN32)
    target_link_libraries(lab3 PRIVATE kernel32)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/data.txt")
    add_custom_command(TARGET lab3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/data.txt"
                "$<TARGET_FILE_DIR:lab3>/data.txt"
        COMMENT "Copying data.txt next to the executable")
endif()

if(MSVC)
    target_compile_options(lab3 PRIVATE /W4)
else()
    target_compile_options(lab3 PRIVATE -Wall -Wextra)
endif()