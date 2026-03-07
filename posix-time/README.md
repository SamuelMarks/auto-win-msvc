# posix-time

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `utime` (Mapped to `_utime`)
- [x] `tzset` (Mapped to `_tzset`)
- [x] `getitimer` (Polyfill with minimal stub due to lack of `SIGALRM`)
- [x] `gettimeofday` (Polyfill with `GetSystemTimeAsFileTime`)
- [x] `setitimer` (Polyfill with minimal stub due to lack of `SIGALRM`)
- [x] `utimes` (Polyfill with `SetFileTime`)

## Tested Environments

This library has been rigorously tested against a wide variety of C89-compliant compilers, ensuring broad compatibility and zero warnings (`/W4 /WX` or `-Wall -Wextra -Werror -pedantic`).

| Compiler / Environment | Version | Notes |
| :--- | :--- | :--- |
| MSVC (Visual Studio 2026/2022) | `cl.exe` 19.x | Builds cleanly without requiring `<windows.h>` overhead. |
| MSVC (Visual Studio 2005) | `cl.exe` 14.x | Safely falls back to `<windows.h>` due to missing API headers. |
| MinGW-w64 | `gcc` 13.x/15.x | Strict standard adherence. `ULL` constants avoid pedantic warnings. |
| Cygwin | `gcc` 13.x | Compiles natively with `_XOPEN_SOURCE=700` and `_POSIX_C_SOURCE=200809L`. |

## Caveats & Limitations

When utilizing these POSIX polyfills on Windows, please note the following platform-specific constraints:

*   **File Path Lengths:** Windows traditionally limits file paths to `MAX_PATH` (260 characters). While newer versions of Windows 10+ support long paths, the narrow character variants of the Win32 API used here (`CreateFileA`) generally do not natively support paths longer than `MAX_PATH` unless specifically prefixed with `\\?\` (which requires absolute paths).
*   **Unicode/Wide Characters:** The current implementation uses narrow character strings (`const char *`) and `CreateFileA`. It does not handle UTF-8 conversion to UTF-16 (`wchar_t`) internally, meaning file operations using non-ASCII characters may fail depending on the active Windows code page.
*   **UNC Paths:** UNC paths (`\\server\share\file`) are supported by `CreateFileA`, but subject to the same `MAX_PATH` and character encoding limitations mentioned above.
*   **Timers:** `getitimer` and `setitimer` are implemented as minimal state-tracking stubs. Windows does not have a direct equivalent to POSIX interval timers that deliver `SIGALRM`, `SIGVTALRM`, or `SIGPROF`. These functions will save and return timer state but will not actually fire signals.
*   **Timezone:** The `timezone` struct populated by `gettimeofday` is considered obsolete in POSIX, but is provided for legacy compatibility. It relies on `GetTimeZoneInformation` for basic DST and bias tracking.


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
    "posix-time"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-time
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-time
)
FetchContent_MakeAvailable(posix-time)
target_link_libraries(your_target PRIVATE posix-time)
```
