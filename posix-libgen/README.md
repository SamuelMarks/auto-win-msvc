# posix-libgen

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `basename`
- [x] `dirname`

## Tested Environments

This library is rigorously tested and verified to build cleanly (`/W4 /WX` or `-Wall -Wextra -Werror -pedantic`) and pass 100% of test assertions across the following environments:

| Environment | Compiler / Version | Build Generator |
| :--- | :--- | :--- |
| **MSVC 2005** | Visual Studio 8 (`cl.exe` 14.0) | NMake Makefiles |
| **MSVC 2022** | Visual Studio 17 (`cl.exe` 19.44) | Visual Studio 17 2022 |
| **MSVC 2026** | Visual Studio 18 (`cl.exe` 19.50) | Ninja |
| **MinGW** | GCC 15.2.0 (MSYS2) | MinGW Makefiles |
| **Cygwin** | GCC 13.4.0 | Unix Makefiles |

## Implementation Notes & Caveats

To ensure maximum portability, strict POSIX compliance, and a minimal footprint, this implementation makes specific design trade-offs:

*   **No `<windows.h>` Dependency**: We avoid `_splitpath_s` and Microsoft CRT path functions entirely. The parsing is done manually to avoid polluting the global namespace with Windows headers, keeping compile times fast and the binary footprint extremely small. It precisely mimics POSIX behavior for edge cases like root directories and trailing slashes.
*   **Strict C89 Compliance**: The code is rigorously written in C89 to seamlessly drop into legacy C codebases without triggering compiler warnings or errors.
*   **Safe CRT Integration**: When built on MSVC 2005 or newer (`_MSC_VER >= 1400`), the library automatically switches to Safe CRT buffer functions (e.g., `strcpy_s`, `strnlen_s`) to improve buffer safety without breaking cross-platform compilation.
*   **Thread Safety**: As permitted by the POSIX standard, both `basename` and `dirname` return a pointer to an internal `static` buffer. **These functions are not thread-safe.** Subsequent calls from the same thread will overwrite the buffer. If thread safety is required, callers must copy the returned string immediately.
*   **Path Length Limits**: Path sizes are strictly bounded by `_MAX_PATH` (typically 260 characters). Paths exceeding this maximum length limit will not be processed correctly and default fallback strings (such as `.`) are returned.
*   **Unicode & UNC Paths**: The implementation operates entirely on narrow `char*` strings (ASCII or the active system ANSI code page). There is currently no support for wide characters (`wchar_t`), native UTF-16 strings, or long Windows UNC paths (e.g., `\\?\C:\...`).

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
    "posix-libgen"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-libgen
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-libgen
)
FetchContent_MakeAvailable(posix-libgen)
target_link_libraries(your_target PRIVATE posix-libgen)
```