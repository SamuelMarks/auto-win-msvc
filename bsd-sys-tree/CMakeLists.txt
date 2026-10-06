
project(bsd-sys-tree LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-tree STATIC ${SOURCES})
    target_include_directories(bsd-sys-tree PUBLIC "include")
else()
    add_library(bsd-sys-tree INTERFACE)
    target_include_directories(bsd-sys-tree INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-sys-tree ${TEST_SOURCES})
target_include_directories(test_bsd-sys-tree PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-sys-tree PRIVATE bsd-sys-tree)
add_test(NAME test_bsd-sys-tree COMMAND test_bsd-sys-tree)
