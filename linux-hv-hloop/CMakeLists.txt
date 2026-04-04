cmake_minimum_required(VERSION 3.10)
project(linux-hv-hloop C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(linux-hv-hloop STATIC src/linux-hv-hloop.c)
target_include_directories(linux-hv-hloop PUBLIC include)

if(MSVC)
    target_compile_definitions(linux-hv-hloop PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
add_executable(test_linux-hv-hloop tests/test.c)
target_link_libraries(test_linux-hv-hloop linux-hv-hloop)
add_test(NAME linux-hv-hloop_test COMMAND test_linux-hv-hloop)
endif()


