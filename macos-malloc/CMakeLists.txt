cmake_minimum_required(VERSION 3.10)
project(macos-malloc C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(macos-malloc STATIC src/macos-malloc.c)
target_include_directories(macos-malloc PUBLIC include)

if(MSVC)
    target_compile_definitions(macos-malloc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

enable_testing()
add_executable(test_macos-malloc tests/test.c)
target_link_libraries(test_macos-malloc macos-malloc)
add_test(NAME macos-malloc_test COMMAND test_macos-malloc)
