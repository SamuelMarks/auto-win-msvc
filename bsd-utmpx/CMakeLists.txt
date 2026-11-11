project(bsd-utmpx LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-utmpx STATIC ${SOURCES})
target_include_directories(bsd-utmpx PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-time/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-utmpx ${TEST_SOURCES})
target_include_directories(test_bsd-utmpx PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-utmpx PRIVATE bsd-utmpx)
add_test(NAME test_bsd-utmpx COMMAND test_bsd-utmpx)
