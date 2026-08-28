cmake_minimum_required(VERSION 3.14)
project(macos-availabilitymacros VERSION 0.1.0 LANGUAGES C)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)

if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    add_compile_options(/WX )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

include(FetchContent)

if(NOT TARGET greatest)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)

endif()




add_library(macos-availabilitymacros src/macos-availabilitymacros.c)
set_target_properties(macos-availabilitymacros PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(macos-availabilitymacros PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(macos-availabilitymacros PUBLIC posix-stdint posix-stdbool)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_macos-availabilitymacros ${TEST_SOURCES})
target_link_libraries(test_macos-availabilitymacros PRIVATE macos-availabilitymacros greatest)
add_test(NAME test_macos-availabilitymacros COMMAND test_macos-availabilitymacros)
endif()

if(MSVC)
    target_compile_options(macos-availabilitymacros PRIVATE )
endif()
