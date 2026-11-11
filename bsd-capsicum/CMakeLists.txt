project(bsd-capsicum LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-capsicum STATIC ${SOURCES})
target_include_directories(bsd-capsicum PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-cdefs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-sysctl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-stat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-pwdgrp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-netdb/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-syslog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-capsicum ${TEST_SOURCES})
target_include_directories(test_bsd-capsicum PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-capsicum PRIVATE bsd-capsicum)
foreach(_dep bsd-sys-sysctl posix-stat posix-pwdgrp posix-netdb posix-syslog)
    if(TARGET ${_dep})
        target_link_libraries(test_bsd-capsicum PRIVATE ${_dep})
    endif()
endforeach()
add_test(NAME test_bsd-capsicum COMMAND test_bsd-capsicum)
