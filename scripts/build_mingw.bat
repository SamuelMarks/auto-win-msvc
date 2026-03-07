@echo off
setlocal
set "TC_NAME=mingw"
set "RES_FILE=%~dp0results_%TC_NAME%.txt"
if exist "%RES_FILE%" del "%RES_FILE%"

set "MINGW="
if exist "C:\msys64\mingw64\bin\gcc.exe" set "MINGW=C:\msys64\mingw64\bin"
if exist "C:\Strawberry\c\bin\gcc.exe" set "MINGW=C:\Strawberry\c\bin"

if "%MINGW%"=="" (
    echo %TC_NAME% skipped: gcc.exe not found.
    exit /b 1
)

set "PATH=%MINGW%;%PATH%"

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

cmake -G "MinGW Makefiles" ..
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
