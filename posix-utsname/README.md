# posix-utsname

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `uname` (Polyfilled using `GetComputerNameA`, `RtlGetVersion`/`GetVersionExA`, and `GetSystemInfo`)

## Tested Environments

This library is rigorously tested to ensure cross-compiler compatibility without generating a single warning, even under strict compiler flags (`/W4`, `/WX`, `-Wall`, `-Wextra`, `-Werror`).

| Compiler / Environment | Version | Result | Notes |
| :--- | :--- | :--- | :--- |
| **MSVC 2026** | 19.50 | ✅ Pass | Strict C89 compliance. |
| **MSVC 2022** | 19.44 | ✅ Pass | Native `windows.h` macro bypass utilized. |
| **MSVC 2005** | 14.00 | ✅ Pass | Full `/WX` compliance including legacy `C4054` casting warnings. |
| **MinGW-w64** | GCC 15.2.0 | ✅ Pass | Full `-Wcast-function-type` compliance. |
| **Cygwin** | GCC 13.4.0 | ✅ Pass | Conditionally runs the native Win32 `uname` polyfill. |

## Implementation Caveats & Details

When relying on this POSIX `utsname` implementation for cross-platform compatibility, developers should note the following constraints specific to this Windows polyfill:

*   **Unicode Support (ANSI Only)**: This library strictly uses the ANSI (`A`-suffixed) Windows APIs (e.g., `GetComputerNameA`, `GetVersionExA`) rather than the Wide (`W`-suffixed) variants. This ensures standard C `char[]` string compatibility with the POSIX `struct utsname`, but limits the `nodename` and `sysname` fields to the local system code page. Multibyte characters outside the active code page may be rendered as question marks (`?`).
*   **Buffer Truncation**: The `_UTSNAME_LENGTH` is defined as `256` bytes (including the null terminator). If the system's underlying representation of the nodename or other fields exceeds 255 characters, it will be strictly truncated to fit the POSIX-compliant struct using Safe CRT (`strncpy_s`) boundaries on modern MSVC compilers, or standard `strncpy` on older ones.
*   **Version Spoofing (RtlGetVersion vs GetVersionEx)**: Starting in Windows 8.1, Microsoft deprecated `GetVersionExA` and began returning spoofed values depending on the application's manifest. To combat this and return the true kernel version, this implementation dynamically loads `RtlGetVersion` from `ntdll.dll` first. If it is unavailable (e.g., on extremely old Windows versions), it safely falls back to `GetVersionExA`.
*   **System Name Constraint**: The `sysname` field is statically populated as `"Windows_NT"` rather than attempting to interpret the OS name dynamically (e.g., "Windows 10", "Windows Server 2022"). This aligns with standard Cygwin and MSYS behavior, providing a predictable OS identifier for build systems and scripts.

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
    "posix-utsname"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-utsname
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-utsname
)
FetchContent_MakeAvailable(posix-utsname)
target_link_libraries(your_target PRIVATE posix-utsname)
```
