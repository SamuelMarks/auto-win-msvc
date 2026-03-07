@echo off
setlocal EnableDelayedExpansion

set "REPORT=%~dp0..\local_tests_results.md"
echo ^| Module ^| MSVC 2005 ^| MSVC 2022 ^| MSVC 2026 ^| MinGW ^| Cygwin ^| > "%REPORT%"
echo ^|---^|---^|---^|---^|---^|---^| >> "%REPORT%"

for /d %%D in ("%~dp0..\posix-*") do (
    set "MOD=%%~nxD"

    set "R_2005=Skipped"
    if exist "%~dp0results_msvc2005.txt" (
        for /f "tokens=1,* delims=:" %%A in ('findstr /b /c:"!MOD! :" "%~dp0results_msvc2005.txt" 2^>nul') do (
            set "VAL=%%B"
            if "!VAL:~0,1!"==" " set "VAL=!VAL:~1!"
            set "R_2005=!VAL!"
        )
    )

    set "R_2022=Skipped"
    if exist "%~dp0results_msvc2022.txt" (
        for /f "tokens=1,* delims=:" %%A in ('findstr /b /c:"!MOD! :" "%~dp0results_msvc2022.txt" 2^>nul') do (
            set "VAL=%%B"
            if "!VAL:~0,1!"==" " set "VAL=!VAL:~1!"
            set "R_2022=!VAL!"
        )
    )

    set "R_2026=Skipped"
    if exist "%~dp0results_msvc2026.txt" (
        for /f "tokens=1,* delims=:" %%A in ('findstr /b /c:"!MOD! :" "%~dp0results_msvc2026.txt" 2^>nul') do (
            set "VAL=%%B"
            if "!VAL:~0,1!"==" " set "VAL=!VAL:~1!"
            set "R_2026=!VAL!"
        )
    )

    set "R_MINGW=Skipped"
    if exist "%~dp0results_mingw.txt" (
        for /f "tokens=1,* delims=:" %%A in ('findstr /b /c:"!MOD! :" "%~dp0results_mingw.txt" 2^>nul') do (
            set "VAL=%%B"
            if "!VAL:~0,1!"==" " set "VAL=!VAL:~1!"
            set "R_MINGW=!VAL!"
        )
    )

    set "R_CYGWIN=Skipped"
    if exist "%~dp0results_cygwin.txt" (
        for /f "tokens=1,* delims=:" %%A in ('findstr /b /c:"!MOD! :" "%~dp0results_cygwin.txt" 2^>nul') do (
            set "VAL=%%B"
            if "!VAL:~0,1!"==" " set "VAL=!VAL:~1!"
            set "R_CYGWIN=!VAL!"
        )
    )

    echo ^| !MOD! ^| !R_2005! ^| !R_2022! ^| !R_2026! ^| !R_MINGW! ^| !R_CYGWIN! ^| >> "%REPORT%"
)

echo Report generated at %REPORT%
