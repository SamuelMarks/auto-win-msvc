cmake_minimum_required(VERSION 3.10)
project(posix-netdb C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-netdb STATIC src/posix-netdb.c)
set_target_properties(posix-netdb PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS" WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(posix-netdb PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-netinet/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-poll/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-ioctl/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-uio/include>
)

if(MSVC)
    target_compile_definitions(posix-netdb PRIVATE )
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
add_executable(test_posix-netdb ${TEST_SOURCES})
target_include_directories(test_posix-netdb PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-netdb PRIVATE posix-netdb greatest)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(test_posix-netdb PRIVATE ws2_32)
endif()
add_test(NAME test_posix-netdb COMMAND test_posix-netdb)

if(MSVC)
    target_compile_options(posix-netdb PRIVATE )
endif()
