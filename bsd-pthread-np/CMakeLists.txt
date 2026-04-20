cmake_minimum_required(VERSION 3.14)
project(bsd-pthread-np VERSION 0.1.0 LANGUAGES C)

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


# Fetch greatest.h
if(NOT TARGET greatest)
include(FetchContent)
FetchContent_Declare(
  greatest
  URL https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)
endif()


# Warnings and ASan
if(MSVC)
    add_compile_options(/W4 /wd4127 /wd4204 /wd4221 /wd4706 /wd4055 /wd4201 /wd4214 /wd4116 /wd4100 /wd4090 /wd4098 /wd4701 /wd4267 /wd4244 /wd4054 /wd4152)
    add_compile_definitions(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wformat=2 -Wshadow -Wcast-qual -Wvla -Wpointer-arith -Wundef)
endif()

add_library(bsd-pthread-np src/bsd-pthread-np.c)
target_include_directories(bsd-pthread-np PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pthread/include")
    target_include_directories(bsd-pthread-np PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pthread/include")
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-pthread-np ${TEST_SOURCES})

FetchContent_GetProperties(greatest SOURCE_DIR greatest_source_dir)
target_include_directories(test_bsd-pthread-np PRIVATE ${greatest_source_dir})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pthread/include")
    target_include_directories(test_bsd-pthread-np PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pthread/include")
endif()

target_link_libraries(test_bsd-pthread-np PRIVATE bsd-pthread-np)
add_test(NAME test_bsd-pthread-np COMMAND test_bsd-pthread-np)
endif()


