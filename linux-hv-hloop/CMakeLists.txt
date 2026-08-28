cmake_minimum_required(VERSION 3.10)
project(linux-hv-hloop C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(linux-hv-hloop STATIC src/linux-hv-hloop.c)
set_target_properties(linux-hv-hloop PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(linux-hv-hloop PUBLIC include)

if(MSVC)
    target_compile_definitions(linux-hv-hloop PRIVATE )
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_linux-hv-hloop ${TEST_SOURCES})
target_link_libraries(test_linux-hv-hloop linux-hv-hloop)
add_test(NAME linux-hv-hloop_test COMMAND test_linux-hv-hloop)
endif()

if(MSVC)
    target_compile_options(linux-hv-hloop PRIVATE )
endif()
