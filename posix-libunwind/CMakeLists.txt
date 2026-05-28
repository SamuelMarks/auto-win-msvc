cmake_minimum_required(VERSION 3.10)
project(posix-libunwind C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-libunwind STATIC src/posix-libunwind.c)
set_target_properties(posix-libunwind PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-libunwind PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-libunwind PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-libunwind ${TEST_SOURCES})
target_link_libraries(test_posix-libunwind posix-libunwind)
add_test(NAME posix-libunwind_test COMMAND test_posix-libunwind)
endif()

if(MSVC)
    target_compile_options(posix-libunwind PRIVATE /wd4273 /wd4141)
endif()
