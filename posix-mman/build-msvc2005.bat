@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
cmake -B build-msvc2005 -G "NMake Makefiles"
cmake --build build-msvc2005
cd build-msvc2005
ctest -V
