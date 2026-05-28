cmake_minimum_required(VERSION 3.10)
project(haiku-os C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(haiku-os STATIC src/haiku-os.c)
set_target_properties(haiku-os PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(haiku-os PUBLIC include)

if(MSVC)
    target_compile_definitions(haiku-os PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_haiku-os ${TEST_SOURCES})
target_link_libraries(test_haiku-os haiku-os)
add_test(NAME haiku-os_test COMMAND test_haiku-os)
endif()

if(MSVC)
    target_compile_options(haiku-os PRIVATE /wd4273 /wd4141)
endif()
