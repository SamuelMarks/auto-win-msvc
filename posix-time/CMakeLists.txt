cmake_minimum_required(VERSION 3.14)
project(posix-time VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

include(FetchContent)
FetchContent_Declare(
  greatest
  URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(posix-time src/posix-time.c)
target_include_directories(posix-time PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-time tests/test.c)
target_link_libraries(test_posix-time PRIVATE posix-time)
target_include_directories(test_posix-time PRIVATE ${greatest_SOURCE_DIR})
add_test(NAME test_posix-time COMMAND test_posix-time)
