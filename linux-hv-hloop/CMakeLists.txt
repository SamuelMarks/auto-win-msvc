cmake_minimum_required(VERSION 3.10)
project(linux-hv-hloop C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(linux-hv-hloop STATIC src/linux-hv-hloop.c)
set_target_properties(linux-hv-hloop PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(linux-hv-hloop PUBLIC include)

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_linux-hv-hloop ${TEST_SOURCES})
target_link_libraries(test_linux-hv-hloop PRIVATE linux-hv-hloop greatest)
add_test(NAME test_linux-hv-hloop COMMAND test_linux-hv-hloop)

if(MSVC)
    target_compile_options(linux-hv-hloop PRIVATE )
endif()
