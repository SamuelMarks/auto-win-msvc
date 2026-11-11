project(bsd-db LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-db STATIC ${SOURCES})
target_include_directories(bsd-db PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-db ${TEST_SOURCES})
target_include_directories(test_bsd-db PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-db PRIVATE bsd-db)
add_test(NAME test_bsd-db COMMAND test_bsd-db)
