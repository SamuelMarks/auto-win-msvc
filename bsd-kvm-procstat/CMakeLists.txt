project(bsd-kvm-procstat LANGUAGES C)
if(NOT MSVC AND NOT (CMAKE_C_COMPILER_ID MATCHES "Watcom"))
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700 _POSIX_C_SOURCE=200809L _DARWIN_C_SOURCE)
endif()

file(GLOB SOURCES "src/*.c")
add_library(bsd-kvm-procstat STATIC ${SOURCES})
target_include_directories(bsd-kvm-procstat PUBLIC
    "include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-libutil/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bsd-sys-queue/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-statfs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../linux-sys-user/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-types/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-time/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sys-resource/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-sockets/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../posix-signal/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    ${MEGA_INCLUDES}
)

enable_testing()
file(GLOB TEST_SOURCES "tests/*.c")
add_executable(test_bsd-kvm-procstat ${TEST_SOURCES})
target_include_directories(test_bsd-kvm-procstat PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(test_bsd-kvm-procstat PRIVATE bsd-kvm-procstat)
if(TARGET linux-sys-statfs)
    target_link_libraries(test_bsd-kvm-procstat PRIVATE linux-sys-statfs)
endif()
add_test(NAME test_bsd-kvm-procstat COMMAND test_bsd-kvm-procstat)
