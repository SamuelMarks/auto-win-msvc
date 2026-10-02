
project(linux-magic LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-magic STATIC ${SOURCES})
    target_include_directories(linux-magic PUBLIC "include")
else()
    add_library(linux-magic INTERFACE)
    target_include_directories(linux-magic INTERFACE "include")
endif()
