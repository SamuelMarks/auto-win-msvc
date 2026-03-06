cmake_minimum_required(VERSION 3.14)
project(posix-spawn VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
    # Enable AddressSanitizer if supported (VS 2019 16.9+)
    # add_compile_options(/fsanitize=address) # Uncomment if using modern MSVC
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_library(posix-spawn src/posix-spawn.c)
target_include_directories(posix-spawn PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

add_executable(test_posix-spawn tests/test.c)
target_include_directories(test_posix-spawn PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-spawn PRIVATE posix-spawn)
add_test(NAME test_posix-spawn COMMAND test_posix-spawn)
