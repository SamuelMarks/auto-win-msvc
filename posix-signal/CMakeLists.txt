cmake_minimum_required(VERSION 3.14)
project(posix-signal VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152 /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-signal src/posix-signal.c)
target_include_directories(posix-signal PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)     

enable_testing()

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(
    greatest
    URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)
endif()



add_executable(test_posix-signal tests/test.c)
target_link_libraries(test_posix-signal PRIVATE posix-signal)
target_include_directories(test_posix-signal PRIVATE ${greatest_SOURCE_DIR})

add_test(NAME test_posix-signal COMMAND test_posix-signal)

