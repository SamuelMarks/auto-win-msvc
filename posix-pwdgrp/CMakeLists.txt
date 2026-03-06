cmake_minimum_required(VERSION 3.14)
project(posix-pwdgrp VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Fetch greatest.h
include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_library(posix-pwdgrp src/posix-pwdgrp.c)
target_include_directories(posix-pwdgrp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(WIN32 OR MINGW)
    target_link_libraries(posix-pwdgrp PUBLIC netapi32 advapi32 userenv ole32)
endif()

enable_testing()
add_executable(test_posix-pwdgrp tests/test.c)

FetchContent_GetProperties(greatest SOURCE_DIR greatest_source_dir)
target_include_directories(test_posix-pwdgrp PRIVATE ${greatest_source_dir})

target_link_libraries(test_posix-pwdgrp PRIVATE posix-pwdgrp)
add_test(NAME test_posix-pwdgrp COMMAND test_posix-pwdgrp)
