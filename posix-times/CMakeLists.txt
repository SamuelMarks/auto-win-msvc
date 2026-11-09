cmake_minimum_required(VERSION 3.14)
project(posix-times VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-times src/posix-times.c)
set_target_properties(posix-times PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-times PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

enable_testing()

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(
    greatest
    GIT_REPOSITORY "https://github.com/SamuelMarks/greatest.git"
    GIT_TAG "master"
)
FetchContent_MakeAvailable(greatest)
if(NOT TARGET greatest)
    add_library(greatest INTERFACE)
    target_include_directories(greatest INTERFACE "${greatest_SOURCE_DIR}")
endif()
endif()



file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-times ${TEST_SOURCES})
target_link_libraries(test_posix-times PRIVATE posix-times)
target_include_directories(test_posix-times PRIVATE ${greatest_SOURCE_DIR})

add_test(NAME test_posix-times COMMAND test_posix-times)

if(MSVC)
    target_compile_options(posix-times PRIVATE )
endif()
