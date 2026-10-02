
project(bsd-sys-cdefs LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-cdefs STATIC ${SOURCES})
    target_include_directories(bsd-sys-cdefs PUBLIC "include")
else()
    add_library(bsd-sys-cdefs INTERFACE)
    target_include_directories(bsd-sys-cdefs INTERFACE "include")
endif()
