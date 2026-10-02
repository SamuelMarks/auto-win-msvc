
project(bsd-stdlib LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-stdlib STATIC ${SOURCES})
    target_include_directories(bsd-stdlib PUBLIC "include")
else()
    add_library(bsd-stdlib INTERFACE)
    target_include_directories(bsd-stdlib INTERFACE "include")
endif()
