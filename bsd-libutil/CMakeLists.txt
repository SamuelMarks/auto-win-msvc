cmake_minimum_required(VERSION 3.14)
project(bsd-libutil VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    add_compile_options(/WX)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

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

if(MSVC AND NOT CMAKE_HOST_WIN32 AND NOT CMAKE_NM)
    find_program(_AWM_HOST_NM NAMES llvm-nm x86_64-w64-mingw32-nm nm PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
    if(_AWM_HOST_NM)
        set(CMAKE_NM "${_AWM_HOST_NM}" CACHE FILEPATH "Path to nm" FORCE)
    endif()
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-libutil STATIC ${SOURCES})
target_include_directories(bsd-libutil PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pwdgrp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-resource/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-kvm-procstat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-queue/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-capsicum/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    ${MEGA_INCLUDES}
)

if(TARGET posix-core)
    target_link_libraries(bsd-libutil PUBLIC posix-core)
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-libutil ${TEST_SOURCES})
target_include_directories(test_bsd-libutil PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-libutil PRIVATE bsd-libutil greatest)
add_test(NAME test_bsd-libutil COMMAND test_bsd-libutil)
