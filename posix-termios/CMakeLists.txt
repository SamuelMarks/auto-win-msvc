cmake_minimum_required(VERSION 3.14)
project(posix-termios VERSION 0.1.0 LANGUAGES C)

# Strict C89 Standard
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
    add_compile_options(/W4 )
    # Enable AddressSanitizer if supported (VS 2019 16.9+)
    # add_compile_options(/fsanitize=address /Zi)
            add_link_options(/DEBUG) # Uncomment if using modern MSVC
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(posix-termios src/posix-termios.c)
set_target_properties(posix-termios PROPERTIES DEFINE_SYMBOL "auto_win_msvc_export_lib_EXPORTS")
target_include_directories(posix-termios PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()

include(FetchContent)
FetchContent_Declare(greatest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../greatest")
FetchContent_MakeAvailable(greatest)

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_posix-termios ${TEST_SOURCES})
target_include_directories(test_posix-termios PRIVATE "${greatest_SOURCE_DIR}")
target_link_libraries(test_posix-termios PRIVATE posix-termios)
add_test(NAME test_posix-termios COMMAND test_posix-termios)

if(MSVC)
    target_compile_options(posix-termios PRIVATE )
endif()
