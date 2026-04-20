cmake_minimum_required(VERSION 3.14)
project(posix-langinfo VERSION 0.1.0 LANGUAGES C)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)

# Warnings and ASan
if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    set(STRICT_C_FLAGS /WX /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(STRICT_C_FLAGS -Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

include(FetchContent)

if(NOT TARGET greatest)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_GetProperties(greatest)
if(NOT greatest_POPULATED)
    FetchContent_MakeAvailable(greatest)
endif()

add_library(greatest INTERFACE)
target_include_directories(greatest INTERFACE ${greatest_SOURCE_DIR})
endif()

add_library(posix-langinfo src/posix-langinfo.c)
target_compile_options(posix-langinfo PRIVATE ${STRICT_C_FLAGS})
target_include_directories(posix-langinfo PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-langinfo ${TEST_SOURCES})
target_link_libraries(test_posix-langinfo PRIVATE posix-langinfo greatest)
target_compile_options(test_posix-langinfo PRIVATE ${STRICT_C_FLAGS})
add_test(NAME test_posix-langinfo COMMAND test_posix-langinfo)
