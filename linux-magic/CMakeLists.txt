
project(linux-magic LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-magic STATIC ${SOURCES})
    target_include_directories(linux-magic PUBLIC "include")
else()
    add_library(linux-magic INTERFACE)
    target_include_directories(linux-magic INTERFACE "include")
endif()
