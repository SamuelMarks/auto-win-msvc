cmake_minimum_required(VERSION 3.14)
project(bsd-kvm-procstat VERSION 0.1.0 LANGUAGES C)

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

if(NOT TARGET linux-sys-statfs AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-statfs/CMakeLists.txt")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-statfs" "${CMAKE_BINARY_DIR}/linux-sys-statfs_build")
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-kvm-procstat STATIC ${SOURCES})
target_include_directories(bsd-kvm-procstat PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-libutil/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-queue/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-statfs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-user/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stdint/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-time/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-resource/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-signal/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    ${_AWM_SIBLING_INCLUDES}
)

if(WIN32)
    target_link_libraries(bsd-kvm-procstat PUBLIC iphlpapi ws2_32 psapi advapi32)
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-kvm-procstat ${TEST_SOURCES})
target_link_libraries(test_bsd-kvm-procstat PRIVATE bsd-kvm-procstat greatest)
if(TARGET linux-sys-statfs)
    target_link_libraries(test_bsd-kvm-procstat PRIVATE linux-sys-statfs)
endif()
add_test(NAME test_bsd-kvm-procstat COMMAND test_bsd-kvm-procstat)
