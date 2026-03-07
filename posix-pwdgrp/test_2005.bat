@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
cl.exe /nologo /W3 /WX /Fetest_msvc2005.exe src\posix-pwdgrp.c tests\test.c /Iinclude /Ibuild\_deps\greatest-src\ /link netapi32.lib advapi32.lib
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
test_msvc2005.exe -v
