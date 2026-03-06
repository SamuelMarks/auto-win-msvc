include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)
message(STATUS "greatest_SOURCE_DIR: ${greatest_SOURCE_DIR}")
