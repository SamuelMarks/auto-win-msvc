
project(bsd-stdlib LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-stdlib STATIC ${SOURCES})
    target_include_directories(bsd-stdlib PUBLIC "include")
else()
    add_library(bsd-stdlib INTERFACE)
    target_include_directories(bsd-stdlib INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-stdlib ${TEST_SOURCES})
target_include_directories(test_bsd-stdlib PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-stdlib PRIVATE bsd-stdlib)
add_test(NAME test_bsd-stdlib COMMAND test_bsd-stdlib)
