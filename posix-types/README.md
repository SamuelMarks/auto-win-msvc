# posix-types

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols



## Tested Environments

This library is rigorously tested to compile with 0 warnings/errors under strict C89 constraints (`-std=c90 -pedantic -Werror` or `/W4 /WX /wd4127`) across the following environments:

| Compiler / Environment | Target OS | Status | Notes |
|------------------------|-----------|--------|-------|
| **MSVC 2005 (VC8)**    | Windows   | ✅ Pass | Native Windows Type Mappings |
| **MSVC 2022**          | Windows   | ✅ Pass | Native Windows Type Mappings |
| **MSVC 2026**          | Windows   | ✅ Pass | Native Windows Type Mappings |
| **MinGW (GCC 13+)**    | Windows   | ✅ Pass | Supplemented native MinGW headers |
| **Cygwin (GCC)**       | Cygwin    | ✅ Pass | Uses native POSIX headers |

## Implications & Caveats

While this library correctly defines POSIX types for Windows, standard Windows environmental constraints still apply to the POSIX functions interacting with them:

* **Path Length Limits (`MAX_PATH`):** Win32 APIs historically limit paths to 260 characters (`MAX_PATH`). While modern Windows 10/11 supports long paths (up to 32,767 characters) via registry opt-in or the `\\?\` prefix, this type layer does not implicitly translate or bypass the 260-character limit for standard CRT functions.
* **Unicode (UTF-8 vs UTF-16):** Standard POSIX code assumes UTF-8 strings. Native Windows APIs expect UTF-16 (`wchar_t`). This library does not perform automatic ANSI/UTF-8 to UTF-16 string conversion. Functions interacting with the filesystem via the C runtime (e.g., `_open`) rely on the active Windows locale, which may not support full Unicode unless the application is explicitly manifested for UTF-8.
* **UNC Paths:** Uniform Naming Convention (UNC) paths (e.g., `\\server\share\file`) are technically supported by Windows APIs, but POSIX application code using forward slashes (`/`) might require normalization depending on the exact CRT function invoked.
* **File Permissions (`mode_t`):** POSIX permissions (e.g., `rwxr-xr-x`) do not directly map to Windows Access Control Lists (ACLs). Windows typically only simulates the read-only and read-write bits. Concepts like execute bits, SUID, and SGID have no direct equivalent in the standard Windows CRT and are effectively ignored.
* **Socket Types (`sa_family_t`, `socklen_t`):** Windows types are leveraged where possible to avoid dragging in massive headers like `<windows.h>`. For example, `sa_family_t` maps directly to `unsigned short` rather than forcing an include of `<winsock2.h>` and inflating the binary size.

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
    "posix-types"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-types
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-types
)
FetchContent_MakeAvailable(posix-types)
target_link_libraries(your_target PRIVATE posix-types)
```
