project(bsd-bitstring LANGUAGES C)

file(GLOB SOURCES "src/*.c")
add_library(bsd-bitstring STATIC ${SOURCES})
target_include_directories(bsd-bitstring PUBLIC "include")

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-bitstring ${TEST_SOURCES})
target_include_directories(test_bsd-bitstring PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-bitstring PRIVATE bsd-bitstring)
add_test(NAME test_bsd-bitstring COMMAND test_bsd-bitstring)
