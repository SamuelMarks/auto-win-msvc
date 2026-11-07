cmake_minimum_required(VERSION 3.14)
project(posix-sys-mman VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-sys-mman INTERFACE)
target_include_directories(posix-sys-mman INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-mman/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)
if(TARGET posix-mman)
    target_link_libraries(posix-sys-mman INTERFACE posix-mman)
endif()
