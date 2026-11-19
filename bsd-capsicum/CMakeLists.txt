cmake_minimum_required(VERSION 3.14)
project(bsd-capsicum VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(MSVC AND NOT CMAKE_HOST_WIN32 AND NOT CMAKE_NM)
    find_program(_AWM_HOST_NM NAMES llvm-nm x86_64-w64-mingw32-nm nm PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
    if(_AWM_HOST_NM)
        set(CMAKE_NM "${_AWM_HOST_NM}" CACHE FILEPATH "Path to nm" FORCE)
    endif()
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
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

foreach(_dep bsd-sys-sysctl posix-stat posix-pwdgrp posix-sockets posix-netdb posix-syslog)
    if(NOT TARGET ${_dep} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../${_dep}/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../${_dep}" "${CMAKE_BINARY_DIR}/${_dep}_build")
    endif()
endforeach()
foreach(_dep bsd-sys-sysctl posix-stat posix-pwdgrp posix-sockets posix-netdb posix-syslog posix-sys-uio posix-sys-ioctl posix-poll)
    if(TARGET ${_dep})
        get_target_property(_dep_type ${_dep} TYPE)
        if(NOT _dep_type STREQUAL "INTERFACE_LIBRARY")
            set_target_properties(${_dep} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        endif()
    endif()
endforeach()

file(GLOB SOURCES "src/*.c")
add_library(bsd-capsicum STATIC ${SOURCES})
target_include_directories(bsd-capsicum PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-cdefs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-sysctl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pwdgrp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-netdb/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-syslog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-capsicum ${TEST_SOURCES})
target_link_libraries(test_bsd-capsicum PRIVATE bsd-capsicum greatest)
foreach(_dep bsd-sys-sysctl posix-stat posix-pwdgrp posix-sockets posix-netdb posix-syslog)
    if(TARGET ${_dep})
        target_link_libraries(test_bsd-capsicum PRIVATE ${_dep})
    endif()
endforeach()
if(WIN32)
    target_link_libraries(test_bsd-capsicum PRIVATE ws2_32)
endif()
add_test(NAME test_bsd-capsicum COMMAND test_bsd-capsicum)
