cmake_minimum_required(VERSION 3.10)
project(posix-unwind C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-unwind STATIC src/posix-unwind.c)
target_include_directories(posix-unwind PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-unwind PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

enable_testing()
add_executable(test_posix-unwind tests/test.c)
target_link_libraries(test_posix-unwind posix-unwind)
add_test(NAME posix-unwind_test COMMAND test_posix-unwind)
