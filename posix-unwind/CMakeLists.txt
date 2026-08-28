cmake_minimum_required(VERSION 3.14)
project(posix-unwind VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings
if(MSVC)
    # Remove default /W3
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 ")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-unwind src/posix-unwind.c)
set_target_properties(posix-unwind PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-unwind PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-unwind ${TEST_SOURCES})
target_include_directories(test_posix-unwind PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-unwind PRIVATE posix-unwind)

add_test(NAME test_posix-unwind COMMAND test_posix-unwind)

if(MSVC)
    target_compile_options(posix-unwind PRIVATE )
endif()
