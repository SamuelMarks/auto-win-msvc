cmake_minimum_required(VERSION 3.10)
project(posix-netdb C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-netdb STATIC src/posix-netdb.c)
set_target_properties(posix-netdb PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-netdb PUBLIC include)

if(MSVC)
    target_compile_definitions(posix-netdb PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-netdb ${TEST_SOURCES})
target_link_libraries(test_posix-netdb posix-netdb)
add_test(NAME posix-netdb_test COMMAND test_posix-netdb)
endif()

if(MSVC)
    target_compile_options(posix-netdb PRIVATE /wd4273 /wd4141)
endif()
