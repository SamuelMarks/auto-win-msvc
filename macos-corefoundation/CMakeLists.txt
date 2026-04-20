cmake_minimum_required(VERSION 3.14)
project(macos-corefoundation VERSION 0.1.0 LANGUAGES C)

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
    add_compile_options(/WX /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

include(FetchContent)

if(NOT TARGET greatest)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(greatest INTERFACE)
target_include_directories(greatest INTERFACE ${greatest_SOURCE_DIR})
endif()




if(NOT TARGET posix-stdint)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint" "posix-stdint_build")
    else()
        FetchContent_Declare(
            posix-stdint
            GIT_REPOSITORY "https://github.com/SamuelMarks/auto-win-msvc.git"
            GIT_TAG "master"
            SOURCE_SUBDIR "posix-stdint"
        )
        FetchContent_MakeAvailable(posix-stdint)
    endif()
endif()

if(NOT TARGET posix-stdbool)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdbool/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdbool" "posix-stdbool_build")
    else()
        FetchContent_Declare(
            posix-stdbool
            GIT_REPOSITORY "https://github.com/SamuelMarks/auto-win-msvc.git"
            GIT_TAG "master"
            SOURCE_SUBDIR "posix-stdbool"
        )
        FetchContent_MakeAvailable(posix-stdbool)
    endif()
endif()

add_library(macos-corefoundation src/macos-corefoundation.c)
target_include_directories(macos-corefoundation PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)  
target_link_libraries(macos-corefoundation PUBLIC posix-stdint posix-stdbool)

if(WIN32)
    target_link_libraries(macos-corefoundation PUBLIC ws2_32)
endif()
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
add_executable(test_macos-corefoundation tests/test.c)
target_link_libraries(test_macos-corefoundation PRIVATE macos-corefoundation greatest)
if(WIN32)
    target_link_libraries(test_macos-corefoundation PRIVATE ws2_32)
endif()
add_test(NAME test_macos-corefoundation COMMAND test_macos-corefoundation)
endif()


