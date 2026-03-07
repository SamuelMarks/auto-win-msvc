@echo off
echo Running all builds in serial...
cd /d "%~dp0"
call build_msvc2005.bat
call build_msvc2022.bat
call build_msvc2026.bat
call build_mingw.bat
call build_cygwin.bat
call generate_report.bat
echo Done.
