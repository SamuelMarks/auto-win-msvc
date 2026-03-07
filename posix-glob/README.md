# posix-glob

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `fnmatch` (Implemented via custom C89 pattern matching)
- [x] `glob` (Implemented using `_findfirst` / `_findnext` on MSVC, `opendir` / `readdir` on POSIX)
- [x] `globfree`
- [x] `wordexp` (Implemented using safe `strtok_s` on MSVC, `strtok_r` on POSIX)
- [x] `wordfree`

## Tested Environments

This library has been rigorously tested across a wide array of environments and compilers, maintaining strict C89 compatibility and 0 compiler warnings (e.g., `/W4 /WX`, `-Wall -Wextra -Werror`):

| Environment / Toolchain | Compiler | Generator / Build System | Architecture | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MSVC 2005** | `cl.exe` v14.0 | NMake Makefiles | x86 | Strict `/W4 /WX /wd4127` |
| **MSVC 2022** | `cl.exe` v19.x | Visual Studio 17 2022 | x64 | Strict `/W4 /WX /wd4127` |
| **MSVC 2026** | `cl.exe` (Latest) | Ninja | x64 | Strict `/W4 /WX /wd4127` |
| **MinGW** | `gcc` | Ninja | x64 | Strict C90 (`-std=c90 -Wall -Wextra -Werror -pedantic`) |
| **Cygwin** | `gcc` | Unix Makefiles | x64 | Strict C90 with `_POSIX_C_SOURCE 200809L` |

## Implementation Notes & Caveats

For those looking to integrate this library, please be aware of the following design decisions and limitations:

- **No `<windows.h>` Inclusion:** To severely reduce binary bloat and namespace pollution, this library completely avoids including `<windows.h>`. Instead, MSVC implementations utilize `<io.h>` and the C Runtime's `_findfirst` / `_findnext` mechanisms.
- **Path Length Restrictions:** Internal buffers for path resolution are hardcoded to 2048 characters to prevent format overflow and avoid dynamic allocation overhead in tight loops. Paths exceeding this length are not supported.
- **ANSI / Multibyte Only (No Explicit Unicode/UTF-16):** To strictly match the POSIX `char*` interface, this library does not utilize Windows `wchar_t` wide-character functions (e.g., `_wfindfirst`). It operates on standard `char` arrays. If your Windows application relies on deep UTF-16 path handling, behavior will be dictated by your active ANSI code page.
- **UNC Path Support:** While `_findfirst` handles standard drive-letter absolute paths (e.g., `C:\`) and relative paths well, complex UNC paths (e.g., `\\?\C:\` or `\\Server\Share`) might exhibit undefined behavior or fail to match depending on the specific Windows CRT implementation.
- **Thread Safety:** The implementation relies on standard C library functions. On MSVC, `strtok_s` is used for thread-safe tokenization in `wordexp`. On POSIX platforms, `strtok_r` is explicitly exposed via `_POSIX_C_SOURCE 200809L` for safe re-entrancy.

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
    "posix-glob"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-glob
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-glob
)
FetchContent_MakeAvailable(posix-glob)
target_link_libraries(your_target PRIVATE posix-glob)
```