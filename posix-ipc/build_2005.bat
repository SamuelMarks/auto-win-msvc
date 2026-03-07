call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
cmake -G "NMake Makefiles" -B build_msvc2005 -S .
cmake --build build_msvc2005 --config Release
ctest --test-dir build_msvc2005 -C Release -V
