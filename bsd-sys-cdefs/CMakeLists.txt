
project(bsd-sys-cdefs LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-cdefs STATIC ${SOURCES})
    target_include_directories(bsd-sys-cdefs PUBLIC "include")
else()
    add_library(bsd-sys-cdefs INTERFACE)
    target_include_directories(bsd-sys-cdefs INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-sys-cdefs ${TEST_SOURCES})
target_include_directories(test_bsd-sys-cdefs PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-sys-cdefs PRIVATE bsd-sys-cdefs)
add_test(NAME test_bsd-sys-cdefs COMMAND test_bsd-sys-cdefs)
