
project(bsd-string LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-string STATIC ${SOURCES})
    target_include_directories(bsd-string PUBLIC "include")
else()
    add_library(bsd-string INTERFACE)
    target_include_directories(bsd-string INTERFACE "include")
endif()

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-string ${TEST_SOURCES})
target_include_directories(test_bsd-string PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-string PRIVATE bsd-string)
add_test(NAME test_bsd-string COMMAND test_bsd-string)
