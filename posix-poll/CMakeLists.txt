cmake_minimum_required(VERSION 3.10)
project(posix-poll C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-poll STATIC src/posix-poll.c)
set_target_properties(posix-poll PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-poll PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(MSVC)
    target_compile_definitions(posix-poll PRIVATE )
endif()

enable_testing()
if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-poll ${TEST_SOURCES})
target_include_directories(test_posix-poll PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-poll PRIVATE posix-poll greatest)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(test_posix-poll PRIVATE ws2_32)
endif()
add_test(NAME test_posix-poll COMMAND test_posix-poll)

if(MSVC)
    target_compile_options(posix-poll PRIVATE )
endif()
