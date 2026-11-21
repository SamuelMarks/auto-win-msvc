cmake_minimum_required(VERSION 3.14)
project(posix-core VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

# Valgrind memory check setup for CTest
find_program(MEMORYCHECK_COMMAND NAMES valgrind)
if(MEMORYCHECK_COMMAND)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=definite,indirect,possible --track-origins=yes -q --error-exitcode=1")
endif()

# Clang Static Analyzer via clang-tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    # set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=-*,clang-analyzer-*,bugprone-*;--warnings-as-errors=*")
endif()

if(MSVC)
    # Native MSVC Static Analyzer
    if(MSVC_VERSION GREATER_EQUAL 1400)
        check_c_compiler_flag("/analyze" HAS_MSVC_ANALYZE)
        if(HAS_MSVC_ANALYZE)
            # add_compile_options(/analyze)
        endif()
    endif()

    # AddressSanitizer and Security features for Modern MSVC
    if(MSVC_VERSION GREATER_EQUAL 1928)
        option(AUTO_WIN_MSVC_ASAN "Enable AddressSanitizer (ASAN)" OFF)
        if(AUTO_WIN_MSVC_ASAN)
            check_c_compiler_flag("/fsanitize=address" HAS_MSVC_ASAN)
            if(HAS_MSVC_ASAN)
                add_compile_options(/fsanitize=address /Zi)
                add_link_options(/DEBUG)
            endif()
        endif()
        check_c_compiler_flag("/guard:cf" HAS_GUARD_CF)
        if(HAS_GUARD_CF)
            add_compile_options(/guard:cf)
            add_link_options(/GUARD:CF)
        endif()
        check_c_compiler_flag("/sdl" HAS_SDL)
        if(HAS_SDL)
            add_compile_options(/sdl)
        endif()
    endif()
else()
    # Unix / Clang / MinGW / Cygwin ASan & UBSan checks
    # Usually enabled on Native Linux/macOS, often problematic on MinGW/Cygwin without DLL path setups
    option(AUTO_WIN_MSVC_ASAN "Enable AddressSanitizer (ASAN)" OFF)
    if(AUTO_WIN_MSVC_ASAN AND NOT WIN32 AND NOT CYGWIN AND NOT MINGW AND NOT EMSCRIPTEN AND NOT CMAKE_SYSTEM_NAME STREQUAL "DOS")
        check_c_compiler_flag("-fsanitize=address" HAS_ASAN)
        check_c_compiler_flag("-fsanitize=undefined" HAS_UBSAN)
        if(HAS_ASAN AND HAS_UBSAN)
            add_compile_options(-fsanitize=address -fsanitize=undefined)
            add_link_options(-fsanitize=address -fsanitize=undefined)
        endif()
    endif()
endif()
# ----------------------------------------------------


if(MSVC)
    add_compile_options(/W4 )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef )
endif()

if(MSVC AND NOT CMAKE_HOST_WIN32 AND NOT CMAKE_NM)
    find_program(_AWM_HOST_NM NAMES llvm-nm x86_64-w64-mingw32-nm nm PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
    if(_AWM_HOST_NM)
        set(CMAKE_NM "${_AWM_HOST_NM}" CACHE FILEPATH "Path to nm" FORCE)
    endif()
endif()

add_library(posix-core src/posix-core.c src/posix_read_write.c src/posix_asprintf.c src/posix_dprintf.c)
set_target_properties(posix-core PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS" WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(posix-core PRIVATE auto_win_msvc_export_lib_EXPORTS)
endif()
target_include_directories(posix-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-pwdgrp/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-fcntl/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-wait/include>
)

enable_testing()
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


if(NOT TARGET posix-sockets)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/CMakeLists.txt")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets posix-sockets_build)
    else()
        include(FetchContent)
        FetchContent_Declare(
            posix-sockets
            GIT_REPOSITORY "https://github.com/SamuelMarks/auto-win-msvc.git"
            GIT_TAG "master"
            SOURCE_SUBDIR "posix-sockets"
        )
        FetchContent_MakeAvailable(posix-sockets)
    endif()
endif()

if(NOT TARGET linux-epoll)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../linux-epoll/CMakeLists.txt")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../linux-epoll linux-epoll_build)
    else()
        include(FetchContent)
        FetchContent_Declare(
            linux-epoll
            GIT_REPOSITORY "https://github.com/SamuelMarks/auto-win-msvc.git"
            GIT_TAG "master"
            SOURCE_SUBDIR "linux-epoll"
        )
        FetchContent_MakeAvailable(linux-epoll)
    endif()
endif()

if(NOT TARGET posix-wait)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-wait/CMakeLists.txt")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../posix-wait posix-wait_build)
    else()
        include(FetchContent)
        FetchContent_Declare(
            posix-wait
            GIT_REPOSITORY "https://github.com/SamuelMarks/auto-win-msvc.git"
            GIT_TAG "master"
            SOURCE_SUBDIR "posix-wait"
        )
        FetchContent_MakeAvailable(posix-wait)
    endif()
endif()

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-core ${TEST_SOURCES})
target_include_directories(test_posix-core PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_posix-core PRIVATE posix-core)
target_link_libraries(posix-core PUBLIC posix-sockets linux-epoll posix-wait)
set_target_properties(posix-core PROPERTIES DEFINE_SYMBOL "posix_core_BUILD_DLL")
if(BUILD_SHARED_LIBS)
    target_compile_definitions(posix-core INTERFACE posix_core_SHARED)
endif()
if(WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "DOS")
    target_link_libraries(posix-core PUBLIC ws2_32 advapi32)
endif()
add_test(NAME test_posix-core COMMAND test_posix-core)

if(WIN32 AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
    # Ensure test can find its dependency DLLs (posix-sockets, posix-sys-uio)
    set_property(TEST test_posix-core PROPERTY ENVIRONMENT "PATH=$<TARGET_FILE_DIR:posix-sockets>\\;$<TARGET_FILE_DIR:posix-sys-uio>\\;$ENV{PATH}")
endif()

if(MSVC)
    target_compile_options(posix-core PRIVATE )
endif()
