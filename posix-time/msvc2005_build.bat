call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
cd /d C:\Users\samue\repos\auto-win-msvc\posix-time
mkdir build_msvc2005
cd build_msvc2005
cmake -G "NMake Makefiles" ..
nmake
ctest --output-on-failure
