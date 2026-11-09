cmake_minimum_required(VERSION 3.10)
project(posix-sys-select C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

add_library(posix-sys-select STATIC src/posix-sys-select.c)
set_target_properties(posix-sys-select PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-sys-select PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

if(MSVC)
    target_compile_definitions(posix-sys-select PRIVATE )
endif()

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
add_executable(test_posix-sys-select ${TEST_SOURCES})
target_include_directories(test_posix-sys-select PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-sys-select PRIVATE posix-sys-select greatest)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(test_posix-sys-select PRIVATE ws2_32)
endif()
add_test(NAME test_posix-sys-select COMMAND test_posix-sys-select)

if(MSVC)
    target_compile_options(posix-sys-select PRIVATE )
endif()
