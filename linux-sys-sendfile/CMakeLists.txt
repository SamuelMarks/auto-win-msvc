cmake_minimum_required(VERSION 3.14)


project(linux-sys-sendfile LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    if(NOT TARGET posix-types)
        add_subdirectory("../posix-types" "${CMAKE_CURRENT_BINARY_DIR}/posix-types")
    endif()
    add_library(linux-sys-sendfile STATIC ${SOURCES})
    target_include_directories(linux-sys-sendfile PUBLIC "include")

else()
    add_library(linux-sys-sendfile INTERFACE)
    target_include_directories(linux-sys-sendfile INTERFACE "include")

endif()

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
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_linux-sys-sendfile ${TEST_SOURCES})
if(TARGET linux-sys-sendfile)
    target_link_libraries(test_linux-sys-sendfile PRIVATE linux-sys-sendfile greatest)
else()
    target_link_libraries(test_linux-sys-sendfile PRIVATE greatest)
endif()
add_test(NAME test_linux-sys-sendfile COMMAND test_linux-sys-sendfile)


target_link_libraries(linux-sys-sendfile PUBLIC posix-types)
target_link_libraries(test_linux-sys-sendfile PRIVATE posix-types)
if(WIN32 AND NOT CYGWIN)
    target_link_libraries(linux-sys-sendfile PUBLIC ws2_32)
endif()
