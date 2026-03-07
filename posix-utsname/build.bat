call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64
cmake -B build_msvc -G Ninja -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build_msvc --clean-first
