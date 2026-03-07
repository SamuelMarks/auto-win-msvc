cmake_minimum_required(VERSION 3.14)
project(posix-dirent VERSION 0.1.0 LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    add_compile_options(/W4 /WX /wd4127 /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_library(posix-dirent src/posix-dirent.c)
target_include_directories(posix-dirent PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-dirent tests/test.c)
target_include_directories(test_posix-dirent PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-dirent PRIVATE posix-dirent)
add_test(NAME test_posix-dirent COMMAND test_posix-dirent)
