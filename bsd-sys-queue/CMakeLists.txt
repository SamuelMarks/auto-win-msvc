
project(bsd-sys-queue LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-sys-queue STATIC ${SOURCES})
    target_include_directories(bsd-sys-queue PUBLIC "include")
else()
    add_library(bsd-sys-queue INTERFACE)
    target_include_directories(bsd-sys-queue INTERFACE "include")
endif()
