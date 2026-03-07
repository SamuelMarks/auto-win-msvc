cmake_minimum_required(VERSION 3.14)
project(posix-utsname VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_library(posix-utsname src/posix-utsname.c)
target_include_directories(posix-utsname PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)

FetchContent_GetProperties(greatest)
if(NOT greatest_POPULATED)
  FetchContent_Populate(greatest)
endif()

add_executable(test_posix-utsname tests/test.c)
target_include_directories(test_posix-utsname PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-utsname PRIVATE posix-utsname)
add_test(NAME test_posix-utsname COMMAND test_posix-utsname)
