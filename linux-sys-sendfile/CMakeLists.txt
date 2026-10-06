cmake_minimum_required(VERSION 3.14)


project(linux-sys-sendfile LANGUAGES C)

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
    FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
    FetchContent_MakeAvailable(greatest)
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
