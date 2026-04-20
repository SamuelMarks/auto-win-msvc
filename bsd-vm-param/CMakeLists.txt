cmake_minimum_required(VERSION 3.14)
project(bsd-vm-param VERSION 0.1.0 LANGUAGES C)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)

if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    add_compile_options(/WX /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

include(FetchContent)

if(NOT TARGET greatest)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(greatest INTERFACE)
target_include_directories(greatest INTERFACE ${greatest_SOURCE_DIR})
endif()




add_library(bsd-vm-param src/bsd-vm-param.c)
target_include_directories(bsd-vm-param PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(bsd-vm-param PUBLIC posix-stdint posix-stdbool)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-vm-param ${TEST_SOURCES})
target_link_libraries(test_bsd-vm-param PRIVATE bsd-vm-param greatest)
add_test(NAME test_bsd-vm-param COMMAND test_bsd-vm-param)
endif()


