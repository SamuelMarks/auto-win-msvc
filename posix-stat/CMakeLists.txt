cmake_minimum_required(VERSION 3.14)
project(posix-stat VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS /wd4127)
    # Enable AddressSanitizer if supported (VS 2019 16.9+)
    # add_compile_options(/fsanitize=address) # Uncomment if using modern MSVC
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Fetch greatest.h
include(FetchContent)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(posix-stat src/posix-stat.c)
target_include_directories(posix-stat PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-stat tests/test.c)

# Include the directory where greatest.h was downloaded
FetchContent_GetProperties(greatest SOURCE_DIR greatest_src)
target_include_directories(test_posix-stat PRIVATE ${greatest_src})

target_link_libraries(test_posix-stat PRIVATE posix-stat)
add_test(NAME test_posix-stat COMMAND test_posix-stat WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})