cmake_minimum_required(VERSION 3.14)
project(greatest)
add_library(greatest INTERFACE)
target_include_directories(greatest INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
