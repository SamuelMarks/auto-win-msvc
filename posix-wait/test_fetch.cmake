cmake_minimum_required(VERSION 3.14)
project(test_fetch)
include(FetchContent)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)
message("Source dir: ${greatest_SOURCE_DIR}")
