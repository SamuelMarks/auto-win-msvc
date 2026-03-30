cmake_minimum_required(VERSION 3.14)
project(posix-alloca VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.c")
add_library(posix-alloca ${SOURCES})
if(NOT SOURCES)
  set_target_properties(posix-alloca PROPERTIES LINKER_LANGUAGE C)
endif()
target_include_directories(posix-alloca PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

include(FetchContent)
FetchContent_Declare(
  greatest
  GIT_REPOSITORY https://github.com/silentbicycle/greatest.git
  GIT_TAG        v1.4.2
)
FetchContent_MakeAvailable(greatest)

add_executable(test_posix-alloca tests/test.c)
target_include_directories(test_posix-alloca PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-alloca PRIVATE posix-alloca)
if(TARGET greatest)
    target_link_libraries(test_posix-alloca PRIVATE greatest)
endif()

enable_testing()
add_test(NAME test_posix-alloca COMMAND test_posix-alloca)
