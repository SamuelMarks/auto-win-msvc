cmake_minimum_required(VERSION 3.14)
project(posix-regex VERSION 0.1.0 LANGUAGES C)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Advanced Tooling & Strictest Compile Options ---
include(CheckCCompilerFlag)

if(MSVC AND MSVC_VERSION LESS 1600)
    set(HAVE_INTTYPES_H 0 CACHE INTERNAL "" FORCE)
    set(HAVE_STDINT_H 0 CACHE INTERNAL "" FORCE)
endif()

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
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
    set(STRICT_CFLAGS /WX)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(STRICT_CFLAGS -Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)
endif()

if(NOT TARGET pcre2-8 AND NOT TARGET pcre2::pcre2-8)
    find_package(pcre2 CONFIG QUIET)
    if(NOT pcre2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            pcre2
            URL https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.44/pcre2-10.44.zip
            DOWNLOAD_EXTRACT_TIMESTAMP true
        )
        set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "" FORCE)
        set(PCRE2_BUILD_PCRE2_16 OFF CACHE BOOL "" FORCE)
        set(PCRE2_BUILD_PCRE2_32 OFF CACHE BOOL "" FORCE)
        set(PCRE2_BUILD_PCRE2POSIX OFF CACHE BOOL "" FORCE)
        set(PCRE2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)

        get_directory_property(_old_opts COMPILE_OPTIONS)
        set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

        FetchContent_MakeAvailable(pcre2)

        set_directory_properties(PROPERTIES COMPILE_OPTIONS "${_old_opts}")
    endif()
endif()

add_library(posix-regex src/posix-regex.c)
set_target_properties(posix-regex PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-regex PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
if(TARGET pcre2::pcre2-8)
    target_link_libraries(posix-regex PRIVATE pcre2::pcre2-8)
else()
    target_link_libraries(posix-regex PRIVATE pcre2-8)
endif()

file(GLOB TEST_SOURCES "tests/*.c")
enable_testing()
add_executable(test_posix-regex ${TEST_SOURCES})
target_link_libraries(test_posix-regex PRIVATE posix-regex greatest)
add_test(NAME test_posix-regex COMMAND test_posix-regex)

if(DEFINED STRICT_CFLAGS)
    target_compile_options(posix-regex PRIVATE ${STRICT_CFLAGS})
    target_compile_options(test_posix-regex PRIVATE ${STRICT_CFLAGS})
endif()
