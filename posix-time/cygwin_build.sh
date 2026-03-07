export PATH=/usr/bin:\$PATH
cd /cygdrive/c/Users/samue/repos/auto-win-msvc/posix-time
mkdir -p build_cygwin
cd build_cygwin
cmake -G "Unix Makefiles" ..
make
./test_posix-time.exe
