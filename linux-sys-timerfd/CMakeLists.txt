
project(linux-sys-timerfd LANGUAGES C)

file(GLOB SOURCES "src/*.c")
if(SOURCES)
    add_library(linux-sys-timerfd STATIC ${SOURCES})
    target_include_directories(linux-sys-timerfd PUBLIC "include")
else()
    add_library(linux-sys-timerfd INTERFACE)
    target_include_directories(linux-sys-timerfd INTERFACE "include")
endif()
