include(FetchContent)
FetchContent_Declare(
    greatest
    URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_GetProperties(greatest)
if(NOT greatest_POPULATED)
    FetchContent_MakeAvailable(greatest)
endif()
message(STATUS "greatest_SOURCE_DIR: ${greatest_SOURCE_DIR}")
