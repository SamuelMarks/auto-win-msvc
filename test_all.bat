@echo off
cd /d "%~dp0"
for /D %%d in (posix-*) do (
  echo Testing %%d
  cd "%%d"
  if exist build rmdir /S /Q build
  cmake -G "Visual Studio 17 2022" -A x64 -B build >nul 2>&1
  cmake --build build --config Release >nul 2>&1
  if errorlevel 1 echo FAILED %%d
  cd ..
)
