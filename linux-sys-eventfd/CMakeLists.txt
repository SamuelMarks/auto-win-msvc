
project(linux-sys-eventfd LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-sys-eventfd STATIC ${SOURCES})
    target_include_directories(linux-sys-eventfd PUBLIC "include")
else()
    add_library(linux-sys-eventfd INTERFACE)
    target_include_directories(linux-sys-eventfd INTERFACE "include")
endif()
