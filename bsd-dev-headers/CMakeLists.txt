project(bsd-dev-headers LANGUAGES C)

file(GLOB SOURCES "src/*.c")
add_library(bsd-dev-headers STATIC ${SOURCES})
target_include_directories(bsd-dev-headers PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-cdefs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-ioctl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    ${MEGA_INCLUDES}
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-dev-headers ${TEST_SOURCES})
target_include_directories(test_bsd-dev-headers PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-dev-headers PRIVATE bsd-dev-headers)
add_test(NAME test_bsd-dev-headers COMMAND test_bsd-dev-headers)
