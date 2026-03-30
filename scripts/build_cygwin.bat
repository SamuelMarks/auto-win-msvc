@echo off
setlocal
set "TC_NAME=cygwin"
set "RES_FILE=%~dp0results_%TC_NAME%.txt"
if exist "%RES_FILE%" del "%RES_FILE%"

set "CYGBIN="
if exist "C:\usr\cygwin64\bin\bash.exe" set "CYGBIN=C:\usr\cygwin64\bin"
if exist "C:\cygwin64\bin\bash.exe" set "CYGBIN=C:\cygwin64\bin"
if exist "C:\cygwin\bin\bash.exe" set "CYGBIN=C:\cygwin\bin"

if "%CYGBIN%"=="" (
    echo %TC_NAME% skipped: bash.exe not found.
    exit /b 1
)

for /d %%D in ("%~dp0..\posix-*") do (
    call :build_module "%%~fD"
)
goto :eof

:build_module
set "MOD_DIR=%~1"
set "MOD_NAME=%~nx1"
echo ==========================================
echo Building %MOD_NAME% with %TC_NAME%
echo ==========================================
cd /d "%MOD_DIR%"
if not exist "build_%TC_NAME%" mkdir "build_%TC_NAME%"
cd "build_%TC_NAME%"

echo #!/bin/bash > run_cygwin.sh
echo export PATH=/usr/bin:$PATH >> run_cygwin.sh
echo bdir="$(cygpath -u "%MOD_DIR%\build_%TC_NAME%")" >> run_cygwin.sh
echo cd "$bdir" >> run_cygwin.sh
echo cmake -G "Unix Makefiles" .. >> run_cygwin.sh
echo if [ $? -ne 0 ]; then exit 1; fi >> run_cygwin.sh
echo cmake --build . >> run_cygwin.sh
echo if [ $? -ne 0 ]; then exit 2; fi >> run_cygwin.sh
echo ctest -V >> run_cygwin.sh
echo if [ $? -ne 0 ]; then exit 3; fi >> run_cygwin.sh
echo exit 0 >> run_cygwin.sh
"%CYGBIN%\tr.exe" -d '\r' < run_cygwin.sh > run_cygwin_lf.sh
move /y run_cygwin_lf.sh run_cygwin.sh >nul

"%CYGBIN%\bash.exe" -l "%MOD_DIR%\build_%TC_NAME%\run_cygwin.sh"
if errorlevel 3 goto :fail_test
if errorlevel 2 goto :fail_build
if errorlevel 1 goto :fail_config

echo %MOD_NAME% : Success >> "%RES_FILE%"
goto :eof

:fail_config
echo %MOD_NAME% : Config Failed >> "%RES_FILE%"
goto :eof

:fail_build
echo %MOD_NAME% : Build Failed >> "%RES_FILE%"
goto :eof

:fail_test
echo %MOD_NAME% : Test Failed >> "%RES_FILE%"
goto :eof
