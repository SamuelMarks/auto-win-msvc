cmake_minimum_required(VERSION 3.14)
project(bsd-db VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
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

file(GLOB SOURCES "src/*.c")
add_library(bsd-db STATIC ${SOURCES})
target_include_directories(bsd-db PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-db ${TEST_SOURCES})
target_link_libraries(test_bsd-db PRIVATE bsd-db greatest)
add_test(NAME test_bsd-db COMMAND test_bsd-db)
