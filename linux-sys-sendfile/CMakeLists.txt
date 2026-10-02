
project(linux-sys-sendfile LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-sys-sendfile STATIC ${SOURCES})
    target_include_directories(linux-sys-sendfile PUBLIC "include")
    target_link_libraries(linux-sys-sendfile PUBLIC posix-types)
else()
    add_library(linux-sys-sendfile INTERFACE)
    target_include_directories(linux-sys-sendfile INTERFACE "include")
    target_link_libraries(linux-sys-sendfile INTERFACE posix-types)
endif()
