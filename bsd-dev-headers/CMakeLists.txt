cmake_minimum_required(VERSION 3.14)
project(bsd-dev-headers VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(MSVC AND NOT CMAKE_HOST_WIN32 AND NOT CMAKE_NM)
    find_program(_AWM_HOST_NM NAMES llvm-nm x86_64-w64-mingw32-nm nm PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
    if(_AWM_HOST_NM)
        set(CMAKE_NM "${_AWM_HOST_NM}" CACHE FILEPATH "Path to nm" FORCE)
    endif()
endif()

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

if(MSVC)
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    add_compile_options(/WX)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
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

if(NOT MEGA_INCLUDES)
    file(GLOB _AWM_SIBLING_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../*/include")
else()
    set(_AWM_SIBLING_INCLUDES ${MEGA_INCLUDES})
endif()

if(NOT TARGET posix-core AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-core/CMakeLists.txt")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../posix-core" "${CMAKE_BINARY_DIR}/posix-core_build")
    foreach(_pc_dep posix-core posix-sys-uio posix-sys-ioctl posix-poll posix-sockets linux-epoll)
        if(TARGET ${_pc_dep})
            get_target_property(_pc_type ${_pc_dep} TYPE)
            if(NOT _pc_type STREQUAL "INTERFACE_LIBRARY")
                set_target_properties(${_pc_dep} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
            endif()
        endif()
    endforeach()
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-dev-headers STATIC ${SOURCES})
target_include_directories(bsd-dev-headers PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-cdefs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-queue/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-ioctl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-netinet/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-arpa/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    ${_AWM_SIBLING_INCLUDES}
)

if(TARGET posix-core)
    target_link_libraries(bsd-dev-headers PUBLIC posix-core)
endif()
if(WIN32)
    target_link_libraries(bsd-dev-headers PUBLIC ws2_32)
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-dev-headers ${TEST_SOURCES})
target_link_libraries(test_bsd-dev-headers PRIVATE bsd-dev-headers greatest)
add_test(NAME test_bsd-dev-headers COMMAND test_bsd-dev-headers)
