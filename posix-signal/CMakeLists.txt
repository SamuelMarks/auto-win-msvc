cmake_minimum_required(VERSION 3.14)
project(posix-signal VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
    add_compile_definitions(_XOPEN_SOURCE=700 _DEFAULT_SOURCE _POSIX_C_SOURCE=200809L)
endif()

add_library(posix-signal src/posix-signal.c)
set_target_properties(posix-signal PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-signal PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()



file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-signal ${TEST_SOURCES})
target_link_libraries(test_posix-signal PRIVATE posix-signal)
target_include_directories(test_posix-signal PRIVATE ${greatest_SOURCE_DIR})

add_test(NAME test_posix-signal COMMAND test_posix-signal)

if(MSVC)
    target_compile_options(posix-signal PRIVATE )
endif()
