
project(linux-err LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-err STATIC ${SOURCES})
    target_include_directories(linux-err PUBLIC "include")
else()
    add_library(linux-err INTERFACE)
    target_include_directories(linux-err INTERFACE "include")
endif()
