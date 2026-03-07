cmake_minimum_required(VERSION 3.14)
project(posix-libgen VERSION 0.1.0 LANGUAGES C)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    add_compile_options(/WX /wd4127)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

include(FetchContent)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_GetProperties(greatest)
if(NOT greatest_POPULATED)
    FetchContent_Populate(greatest)
endif()

add_library(greatest INTERFACE)
target_include_directories(greatest INTERFACE ${greatest_SOURCE_DIR})

add_library(posix-libgen src/posix-libgen.c)
target_include_directories(posix-libgen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-libgen tests/test.c)
target_link_libraries(test_posix-libgen PRIVATE posix-libgen greatest)
add_test(NAME test_posix-libgen COMMAND test_posix-libgen)
