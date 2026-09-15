cmake_minimum_required(VERSION 3.10)
project(posix-netdb C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-netdb STATIC src/posix-netdb.c)
set_target_properties(posix-netdb PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-netdb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(MSVC)
    target_compile_definitions(posix-netdb PRIVATE )
endif()

enable_testing()
if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-netdb ${TEST_SOURCES})
target_include_directories(test_posix-netdb PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-netdb PRIVATE posix-netdb greatest)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(test_posix-netdb PRIVATE ws2_32)
endif()
add_test(NAME test_posix-netdb COMMAND test_posix-netdb)

if(MSVC)
    target_compile_options(posix-netdb PRIVATE )
endif()
