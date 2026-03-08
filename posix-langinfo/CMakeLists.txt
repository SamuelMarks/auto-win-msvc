cmake_minimum_required(VERSION 3.10)
project(posix-langinfo C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-langinfo STATIC src/posix-langinfo.c)
target_include_directories(posix-langinfo PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-langinfo PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

enable_testing()
add_executable(test_posix-langinfo tests/test.c)
target_link_libraries(test_posix-langinfo posix-langinfo)
add_test(NAME posix-langinfo_test COMMAND test_posix-langinfo)
