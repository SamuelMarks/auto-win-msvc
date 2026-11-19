cmake_minimum_required(VERSION 3.14)
project(posix-pthread VERSION 0.1.0 LANGUAGES C)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

if(MSVC AND NOT CMAKE_HOST_WIN32 AND NOT CMAKE_NM)
    find_program(_AWM_HOST_NM NAMES llvm-nm x86_64-w64-mingw32-nm nm PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
    if(_AWM_HOST_NM)
        set(CMAKE_NM "${_AWM_HOST_NM}" CACHE FILEPATH "Path to nm" FORCE)
    endif()
endif()

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_GNU_SOURCE _DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

# Valgrind memory check setup for CTest
find_program(MEMORYCHECK_COMMAND NAMES valgrind)
if(MEMORYCHECK_COMMAND)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all --track-origins=yes -q --error-exitcode=1")
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
    if(NOT WIN32 AND NOT CYGWIN AND NOT MINGW)
        check_c_compiler_flag("-fsanitize=address" HAS_ASAN)
        check_c_compiler_flag("-fsanitize=undefined" HAS_UBSAN)
        if(HAS_ASAN AND HAS_UBSAN)
            add_compile_options(-fsanitize=address -fsanitize=undefined)
            add_link_options(-fsanitize=address -fsanitize=undefined)
        endif()
    endif()
endif()
# ----------------------------------------------------


# Warnings and ASan
if(MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    add_compile_options(/W4 /WX)
    add_link_options(/DEBUG)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-pthread src/posix-pthread.c)
set_target_properties(posix-pthread PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS" WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(posix-pthread PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-semaphore/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)
if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(posix-pthread PUBLIC Threads::Threads)
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
add_executable(test_posix-pthread ${TEST_SOURCES})
target_link_libraries(test_posix-pthread PRIVATE posix-pthread)
target_include_directories(test_posix-pthread PRIVATE ${greatest_SOURCE_DIR})
add_test(NAME test_posix-pthread COMMAND test_posix-pthread)

if(MSVC)
    target_compile_options(posix-pthread PRIVATE )
endif()
