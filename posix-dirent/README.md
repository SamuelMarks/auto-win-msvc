# posix-dirent

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `opendir` (via CRT `_findfirst`)
- [x] `readdir` (via CRT `_findnext`)
- [x] `closedir` (via CRT `_findclose`)
- [x] `rewinddir` (via stream reset)
- [x] `seekdir` (via rewind and advance)
- [x] `telldir` (via internal offset tracking)
- [x] `scandir` (custom dynamic array implementation)
- [x] `alphasort` (via `strcmp`)

## Tested Environments

This library is exhaustively tested with 100% test coverage and compiles cleanly with zero warnings (e.g., `/W4 /WX /Za` on MSVC, `-Wall -Wextra -Werror -pedantic` on GCC) across a wide range of C89 compilers:

| Compiler / Environment | Architecture | Status |
| ---------------------- | ------------ | ------ |
| **MSVC 2026**          | x64          | ✅ Pass |
| **MSVC 2022**          | x64          | ✅ Pass |
| **MSVC 2005 (VC8)**    | x86          | ✅ Pass |
| **MinGW (GCC 13.2.0)** | x64          | ✅ Pass |
| **Cygwin (GCC 13.4.0)**| x64          | ✅ Pass |

## Implementation Notes & Caveats

- **Zero `<windows.h>` Inclusion:** The implementation relies purely on the secure MS C Runtime (`<io.h>`, `_findfirst`, `_findnext`), intentionally avoiding `<windows.h>` and `WIN32_FIND_DATA`. This heavily reduces binary bloat and namespace pollution, allowing it to be safely included anywhere.
- **Path Length Constraints:** Fixed to the traditional Windows `MAX_PATH` (260 characters). Very long file paths (e.g., those using the `\\?\` prefix) may exceed the buffer bounds of standard `dirent` structures unless specifically handled by the calling application.
- **Character Encoding (ASCII/ANSI vs Unicode):** This library implements the standard narrow-character POSIX interface (`char*`). It maps to `_findfirst`, meaning it interprets strings based on the active system ANSI code page. There is currently no direct wide-character (`wchar_t` / `_wfindfirst`) UTF-16 polyfill mapped here.
- **UNC Paths:** Supported natively as long as the underlying CRT `_findfirst` resolves them and they fit within the 260-character limit.
- **Thread Safety:** As is standard for many POSIX `dirent` implementations, the `struct dirent` returned by `readdir()` is stored *inside* the `DIR` handle. It is safe to iterate different directory streams on different threads, but iterating the *same* `DIR*` stream concurrently across multiple threads is not thread-safe.

## Current Status & Future Plans

**Current Status:**
- The `auto-win-msvc` monorepo has been successfully scaffolded into 18 distinct, modular CMake projects.
- All standard POSIX headers and types are generated and strictly C89 compliant.
- `posix-dirent` is fully implemented, verified, and strictly mapping to the POSIX/Linux standard interfaces.

**Future Plans:**
- **AI-Driven Iteration:** Iteratively implement all remaining stubbed polyfills across the other 17 modules, maintaining 0 compiler warnings (`/W4 /WX`) and strict C89 compliance.
- **cdd-c Integration:** Expand `cdd-c` into a Concrete Syntax Tree (CST) weaver (as outlined in `cdd-c-expansion.md`). This will allow automated, byte-for-byte precise injection of `auto-win-msvc` polyfills and standard `#ifdef _MSC_VER` guards directly into legacy C codebases.

## Installation

### Vcpkg
Add to your `vcpkg.json`:
```json
{
  "dependencies": [
    "posix-dirent"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-dirent
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-dirent
)
FetchContent_MakeAvailable(posix-dirent)
target_link_libraries(your_target PRIVATE posix-dirent)
```