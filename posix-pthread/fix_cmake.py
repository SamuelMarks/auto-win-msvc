with open('CMakeLists.txt', 'r') as f:
    cm = f.read()

if 'FetchContent' not in cm:
    cm = cm.replace('enable_testing()', """include(FetchContent)
FetchContent_Declare(
    greatest
    URL "https://raw.githubusercontent.com/SamuelMarks/greatest/cmake-and-msvc/greatest.h"
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(greatest)

enable_testing()""")

    cm = cm.replace('target_link_libraries(test_posix-pthread PRIVATE posix-pthread)',
                    'target_link_libraries(test_posix-pthread PRIVATE posix-pthread)\ntarget_include_directories(test_posix-pthread PRIVATE ${greatest_SOURCE_DIR})')

with open('CMakeLists.txt', 'w') as f:
    f.write(cm)

