cmake_minimum_required(VERSION 3.14)
project(posix-ipc VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_library(posix-ipc src/posix-ipc.c)
target_include_directories(posix-ipc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_posix-ipc tests/test.c)
target_include_directories(test_posix-ipc PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-ipc PRIVATE posix-ipc)
add_test(NAME test_posix-ipc COMMAND test_posix-ipc)
