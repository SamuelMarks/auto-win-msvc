cmake_minimum_required(VERSION 3.10)
project(posix-poll C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-poll STATIC src/posix-poll.c)
target_include_directories(posix-poll PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-poll PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-poll ${TEST_SOURCES})
target_link_libraries(test_posix-poll posix-poll)
add_test(NAME posix-poll_test COMMAND test_posix-poll)
endif()
