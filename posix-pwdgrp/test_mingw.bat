@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
gcc.exe -Wall -Werror -Iinclude -Ibuild/_deps/greatest-src/ -o test_mingw.exe src/posix-pwdgrp.c tests/test.c -lnetapi32 -ladvapi32
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
test_mingw.exe -v
