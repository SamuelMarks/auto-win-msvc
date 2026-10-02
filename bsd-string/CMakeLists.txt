
project(bsd-string LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(bsd-string STATIC ${SOURCES})
    target_include_directories(bsd-string PUBLIC "include")
else()
    add_library(bsd-string INTERFACE)
    target_include_directories(bsd-string INTERFACE "include")
endif()
