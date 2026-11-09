cmake_minimum_required(VERSION 3.14)


project(linux-err LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-err STATIC ${SOURCES})
    target_include_directories(linux-err PUBLIC "include")
else()
    add_library(linux-err INTERFACE)
    target_include_directories(linux-err INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_linux-err ${TEST_SOURCES})
if(TARGET linux-err)
    target_link_libraries(test_linux-err PRIVATE linux-err greatest)
else()
    target_link_libraries(test_linux-err PRIVATE greatest)
endif()
add_test(NAME test_linux-err COMMAND test_linux-err)
if(NOT TARGET greatest)
    include(FetchContent)
    FetchContent_Declare(
        greatest
        GIT_REPOSITORY "https://github.com/SamuelMarks/greatest.git"
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(greatest)
    if(NOT TARGET greatest)
        add_library(greatest INTERFACE)
        target_include_directories(greatest INTERFACE "${greatest_SOURCE_DIR}")
    endif()
endif()
