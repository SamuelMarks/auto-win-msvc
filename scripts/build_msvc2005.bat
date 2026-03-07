@echo off
setlocal
set "TC_NAME=msvc2005"
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat"
set "RES_FILE=%~dp0results_%TC_NAME%.txt"

if exist "%RES_FILE%" del "%RES_FILE%"

if not exist "%VCVARS%" (
    echo %TC_NAME% skipped: vcvarsall.bat not found.
    exit /b 1
)

call "%VCVARS%" x86

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

cmake -G "NMake Makefiles" ..
if errorlevel 1 goto :fail_config

cmake --build .
if errorlevel 1 goto :fail_build

ctest -V -C Debug
if errorlevel 1 goto :fail_test

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
