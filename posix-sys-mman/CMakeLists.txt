cmake_minimum_required(VERSION 3.10)
project(posix-sys-mman C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(posix-sys-mman INTERFACE)
target_include_directories(posix-sys-mman INTERFACE include)
