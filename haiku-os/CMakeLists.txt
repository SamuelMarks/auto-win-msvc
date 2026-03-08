cmake_minimum_required(VERSION 3.10)
project(haiku-os C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(haiku-os STATIC src/haiku-os.c)
target_include_directories(haiku-os PUBLIC include)

if(MSVC)
    target_compile_definitions(haiku-os PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

enable_testing()
add_executable(test_haiku-os tests/test.c)
target_link_libraries(test_haiku-os haiku-os)
add_test(NAME haiku-os_test COMMAND test_haiku-os)
