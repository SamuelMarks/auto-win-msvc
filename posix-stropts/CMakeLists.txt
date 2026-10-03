cmake_minimum_required(VERSION 3.10)
project(posix-stropts C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-stropts STATIC src/posix-stropts.c)
set_target_properties(posix-stropts PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-stropts PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

if(MSVC)
    target_compile_definitions(posix-stropts PRIVATE )
    target_compile_options(posix-stropts PRIVATE )
endif()
