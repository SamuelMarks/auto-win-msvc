cmake_minimum_required(VERSION 3.14)
project(posix-unwind VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings
if(MSVC)
    # Remove default /W3
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152 /D_CRT_SECURE_NO_WARNINGS")
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-unwind src/posix-unwind.c)
target_include_directories(posix-unwind PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

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

add_executable(test_posix-unwind tests/test.c)
target_include_directories(test_posix-unwind PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-unwind PRIVATE posix-unwind)

add_test(NAME test_posix-unwind COMMAND test_posix-unwind)
