cmake_minimum_required(VERSION 3.10)
project(posix-sys-select C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-sys-select STATIC src/posix-sys-select.c)
target_include_directories(posix-sys-select PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-sys-select PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-sys-select ${TEST_SOURCES})
target_include_directories(test_posix-sys-select PRIVATE ../greatest)
target_link_libraries(test_posix-sys-select posix-sys-select)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(test_posix-sys-select ws2_32)
endif()
add_test(NAME posix-sys-select_test COMMAND test_posix-sys-select)
endif()
