cmake_minimum_required(VERSION 3.14)
project(posix-mman VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    # Remove default /W3
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /wd4127 /D_CRT_SECURE_NO_WARNINGS")
    # Enable Address Sanitizer for modern MSVC (VS 2019 16.9+)
    # if(MSVC_VERSION GREATER_EQUAL 1928)
    #    add_compile_options(/fsanitize=address /Zi)
    #    add_link_options(/DEBUG)
    # endif()
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
    # Enable ASan on Unix platforms as well for testing, skip for MinGW
    if(NOT MINGW AND NOT CYGWIN)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif()

add_library(posix-mman src/posix-mman.c)
target_include_directories(posix-mman PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    greatest
    URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_executable(test_posix-mman tests/test.c)
target_include_directories(test_posix-mman PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-mman PRIVATE posix-mman)

if(MSVC)
    target_link_libraries(posix-mman PUBLIC Advapi32)
endif()

add_test(NAME test_posix-mman COMMAND test_posix-mman)
