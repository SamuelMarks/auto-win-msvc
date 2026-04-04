cmake_minimum_required(VERSION 3.10)
project(posix-netdb C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-netdb STATIC src/posix-netdb.c)
target_include_directories(posix-netdb PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-netdb PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
add_executable(test_posix-netdb tests/test.c)
target_link_libraries(test_posix-netdb posix-netdb)
add_test(NAME posix-netdb_test COMMAND test_posix-netdb)
endif()


