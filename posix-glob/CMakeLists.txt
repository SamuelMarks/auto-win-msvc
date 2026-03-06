cmake_minimum_required(VERSION 3.14)
project(posix-glob VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(posix-glob src/posix-glob.c)
target_include_directories(posix-glob PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-glob tests/test.c)
target_link_libraries(test_posix-glob PRIVATE posix-glob)
target_include_directories(test_posix-glob PRIVATE ${greatest_SOURCE_DIR})
add_test(NAME test_posix-glob COMMAND test_posix-glob)
