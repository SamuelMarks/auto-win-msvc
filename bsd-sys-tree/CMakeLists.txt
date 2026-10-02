
project(bsd-sys-tree LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-tree STATIC ${SOURCES})
    target_include_directories(bsd-sys-tree PUBLIC "include")
else()
    add_library(bsd-sys-tree INTERFACE)
    target_include_directories(bsd-sys-tree INTERFACE "include")
endif()
