# posix-syslog

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `closelog` (Polyfilled with DeregisterEventSource)
- [x] `openlog` (Polyfilled with RegisterEventSourceA)
- [x] `setlogmask` (Implemented)
- [x] `syslog` (Polyfilled with ReportEventA)

## Supported Environments

This library has been rigorously tested and verified to compile and run with 0 warnings and 0 errors under strict C89 modes across the following environments:

| Toolchain | Version | Flags Tested | Notes |
| :--- | :--- | :--- | :--- |
| **MSVC 2022 / 2026** | `v143` (19.44+) | `/W4 /WX /D_CRT_SECURE_NO_WARNINGS` | Uses modern safe CRT extensions (`strncpy_s`, `vsnprintf_s`) where available. |
| **MSVC 2005** | `v80` (14.00+) | `/W4 /WX /Za /D_CRT_SECURE_NO_WARNINGS` | Compiles flawlessly under strict C89 (`/Za`) without pulling in `<windows.h>`. |
| **MinGW-w64** | `GCC 13.2.0` | `-Wall -Wextra -Werror -pedantic -std=c89` | Seamlessly uses manual `WIN_` typedefs and explicit `Advapi32.dll` imports. |
| **Cygwin** | `GCC 13.4.0` | `-Wall -Wextra -Werror -pedantic -std=c89` | Compiles non-Windows stubs and natively utilizes `stderr` when `LOG_PERROR` is set. |

## Implementation Notes & Caveats

To ensure maximum portability, minimal binary bloat, and fast compilation times, this library employs several strict design choices:

- **No `<windows.h>` Include:** The implementation meticulously avoids `#include <windows.h>`. Instead, it uses safe, manual typedefs (e.g., `WIN_HANDLE`, `WIN_WORD`) and direct `__declspec(dllimport)` declarations for `Advapi32.dll` functions. This drastically reduces header expansion and namespace pollution.
- **Strict C89 Compliance:** All local variables are declared at the beginning of their respective blocks. 
- **Safe CRT Fallbacks:** Buffer safety relies on MSVC's secure CRT functions (e.g., `vsnprintf_s`, `strncpy_s`) guarded by `#if defined(_MSC_VER) && _MSC_VER >= 1400`. If compiled on older MSVC versions or non-MSVC Windows compilers (like MinGW), standard C89 fallbacks (`_vsnprintf` or `vsprintf`) are automatically utilized.
- **ASCII/ANSI Only:** This library relies exclusively on the ANSI versions of the Windows Event Log APIs (`RegisterEventSourceA`, `ReportEventA`). **Unicode (`wchar_t`, UTF-16) is not supported.** Identifiers and log messages must be single-byte characters. 
- **Buffer Limitations:** The `syslog` format buffer is statically sized to `4096` bytes. Any formatted message exceeding this length will be cleanly truncated (via `_TRUNCATE` when safe CRT is available) or discarded if standard `vsprintf` encounters a buffer overrun risk (though the latter relies on the caller to format responsibly in environments lacking safe `snprintf`).
- **Event Log Registration:** The library uses `"Application"` as a fallback event source name if `openlog` is called without an `ident` string, or if `ident` is `NULL`. 
- **Non-Windows Platforms:** On platforms where `_WIN32` is not defined (e.g., native Linux or Cygwin), the library acts as a minimal shim. Calls to `syslog` will only output to `stderr` (if `LOG_PERROR` was specified in `openlog`) and will not attempt to link or use any system logging daemon.

## Current Status & Future Plans

**Current Status:**
- The `auto-win-msvc` monorepo has been successfully scaffolded into 18 distinct, modular CMake projects.
- All standard POSIX headers and types are generated and strictly C89 compliant.
- Simple functions with direct MSVC equivalents (e.g., `open` -> `_open`) are fully mapped via macros.
- Complex POSIX APIs requiring polyfills (e.g., `mmap`, `pthreads`, `dirent`) are currently scaffolded as `ENOSYS` stubs, with their target Win32 APIs documented in `mappings.json` files.
- Test files and build systems (CMake and vcpkg) are in place.

**Future Plans:**
- **AI-Driven Iteration:** Iteratively implement all stubbed polyfills using native Win32 APIs across the 18 modules, maintaining 0 compiler warnings (`/W4 /WX`) and strict C89 compliance.
- **cdd-c Integration:** Expand `cdd-c` into a Concrete Syntax Tree (CST) weaver (as outlined in `cdd-c-expansion.md`). This will allow automated, byte-for-byte precise injection of `auto-win-msvc` polyfills and standard `#ifdef _MSC_VER` guards directly into legacy C codebases.

## Installation

### Vcpkg
Add to your `vcpkg.json`:
```json
{
  "dependencies": [
    "posix-syslog"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-syslog
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-syslog
)
FetchContent_MakeAvailable(posix-syslog)
target_link_libraries(your_target PRIVATE posix-syslog)
```
