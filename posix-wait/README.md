# posix-wait

C89 POSIX compatibility layer for Windows (MSVC/MinGW) and Cygwin.

## Implemented Symbols
- [x] `wait` (Polyfilled using `waitpid(-1, ...)`)
- [x] `waitpid` (Polyfilled using `OpenProcess` and `WaitForSingleObject`)
- [x] `waitid` (Polyfilled using `waitpid`)
- [x] `cwait` (Mapped to `waitpid` to align with standard POSIX wait behavior)

## Tested Environments

This library is continuously tested across a wide range of toolchains to ensure strict C89 compliance and warning-free compilation.

| Environment | Version / Toolchain | Status | Notes |
|---|---|---|---|
| **MSVC 2005** | VS 8.0 (NMake) | ✅ Passing | Uses legacy `<windows.h>` fallback |
| **MSVC 2022** | VS 17.x (Ninja) | ✅ Passing | Strict granular headers, zero warnings |
| **MSVC 2026** | VS 18.x (Ninja) | ✅ Passing | Strict granular headers, zero warnings |
| **MinGW-w64** | GCC 13+ (Ninja) | ✅ Passing | Utilizes modern Windows API definitions |
| **Cygwin64** | GCC 13+ (Ninja) | ✅ Passing | Custom `waitid` polyfill over Cygwin's `waitpid` |

## Implementation Notes & Caveats

To cleanly emulate POSIX process waiting mechanisms on Windows, several design choices and limitations exist:

- **Process Groups (`pid <= 0`)**: Windows does not natively support POSIX process groups in the same way Unix does. Passing `0`, `< -1`, or `-1` (for `wait` or `waitid(P_ALL)`) is partially restricted. Waiting for *any* child (`-1`) without explicitly providing a process handle is not reliably supported by the underlying Win32 API without maintaining a complex internal process tree. Currently, passing `pid <= 0` will yield `ECHILD`.
- **Exit Status Shifting**: The exit code retrieved via `GetExitCodeProcess` is artificially shifted by 8 bits left (`exit_code << 8`) before being written to the `stat_loc` pointer. This directly simulates the POSIX status byte layout, ensuring that standard macros like `WIFEXITED(status)` and `WEXITSTATUS(status)` work identically to Linux/BSD without modification.
- **Signals and Stopped Processes**: POSIX macros such as `WUNTRACED`, `WCONTINUED`, `WIFSIGNALED`, `WTERMSIG`, `WIFSTOPPED`, and `WSTOPSIG` are defined for API compatibility. However, Windows processes do not emit POSIX signals or enter stopped/continued states natively in a way that `WaitForSingleObject` catches. The wait will only conclude upon process termination or timeout.
- **Cygwin `waitid` Polyfill**: While Cygwin implements robust POSIX `wait` and `waitpid` functions, it fundamentally lacks a native `waitid` implementation and `idtype_t` enumeration. This library provides a polyfill that maps `waitid` back onto Cygwin's `waitpid`.
- **Granular Windows Headers**: For modern MSVC versions (`_MSC_VER >= 1900`), this library deliberately avoids including the monolithic `<windows.h>` header. Instead, it relies on granular headers like `<minwindef.h>`, `<processthreadsapi.h>`, `<synchapi.h>`, and `<handleapi.h>`. This significantly reduces namespace pollution and compilation times.

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
    "posix-wait"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-wait
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-wait
)
FetchContent_MakeAvailable(posix-wait)
target_link_libraries(your_target PRIVATE posix-wait)
```