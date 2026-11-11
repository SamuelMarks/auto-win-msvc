project(bsd-fts LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-fts STATIC ${SOURCES})
target_include_directories(bsd-fts PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-dirent/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-fts ${TEST_SOURCES})
target_include_directories(test_bsd-fts PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-fts PRIVATE bsd-fts)
if(TARGET posix-dirent)
    target_link_libraries(test_bsd-fts PRIVATE posix-dirent)
endif()
if(TARGET posix-stat)
    target_link_libraries(test_bsd-fts PRIVATE posix-stat)
endif()
add_test(NAME test_bsd-fts COMMAND test_bsd-fts)
