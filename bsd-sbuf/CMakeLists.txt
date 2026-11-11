project(bsd-sbuf LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-sbuf STATIC ${SOURCES})
target_include_directories(bsd-sbuf PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-sbuf ${TEST_SOURCES})
target_include_directories(test_bsd-sbuf PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-sbuf PRIVATE bsd-sbuf)
add_test(NAME test_bsd-sbuf COMMAND test_bsd-sbuf)
