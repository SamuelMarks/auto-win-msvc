cmake_minimum_required(VERSION 3.14)
project(posix-core VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wno-unused-parameter)
endif()

add_library(posix-core src/posix-core.c)
target_include_directories(posix-core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_executable(test_posix-core tests/test.c)
target_include_directories(test_posix-core PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-core PRIVATE posix-core)
add_test(NAME test_posix-core COMMAND test_posix-core)
