
project(bsd-sys-queue LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-queue STATIC ${SOURCES})
    target_include_directories(bsd-sys-queue PUBLIC "include")
else()
    add_library(bsd-sys-queue INTERFACE)
    target_include_directories(bsd-sys-queue INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-sys-queue ${TEST_SOURCES})
target_include_directories(test_bsd-sys-queue PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-sys-queue PRIVATE bsd-sys-queue)
add_test(NAME test_bsd-sys-queue COMMAND test_bsd-sys-queue)
