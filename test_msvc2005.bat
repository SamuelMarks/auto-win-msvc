@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
mkdir test_build_2005
cd test_build_2005
echo cmake_minimum_required(VERSION 3.0) > CMakeLists.txt
echo project(dummy C) >> CMakeLists.txt
echo int main() { return 0; } > main.c
echo add_executable(dummy main.c) >> CMakeLists.txt
cmake -G "NMake Makefiles" ..
cmake --build .
