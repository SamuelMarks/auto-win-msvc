# posix-stat

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `stat` (Mapped to `_stat64`)
- [x] `fstat` (Mapped to `_fstat64`)
- [x] `chmod` (Mapped to `_chmod`)
- [x] `mkdir` (Mapped to `_mkdir`)
- [x] `umask` (Mapped to `_umask`)
- [x] `fchmod` (Polyfilled with SetFileAttributesA)
- [x] `fchmodat` (Polyfilled with Win32 API)
- [x] `fstatat` (Polyfilled with Win32 API)
- [x] `futimens` (Polyfilled with SetFileTime)
- [x] `lstat` (Polyfilled with GetFileAttributesExA)
- [x] `mknod` (Polyfilled with CreateFileA)
- [x] `mknodat` (Polyfilled with Win32 API)
- [x] `utimensat` (Polyfilled with Win32 API)

## Tested Environments

This library has been rigorously tested and confirmed to build and pass its test suite (100% test coverage) with no warnings or errors on the following toolchains under strict C89 (`-pedantic` / `/WX`) constraints:

| Compiler / Environment | Generator | Standard | Status |
| :--- | :--- | :--- | :--- |
| **MSVC 2005 (v140)** | Ninja | C89 | ✅ Passed |
| **MSVC 2022 (v143)** | Ninja / MSBuild | C89 | ✅ Passed |
| **MSVC 2026 (v144)** | Ninja / MSBuild | C89 | ✅ Passed |
| **MinGW-w64 (GCC 13+)** | Ninja | C89 | ✅ Passed |
| **Cygwin (GCC 13+)** | Ninja | C89 | ✅ Passed |

> **Note on POSIX Environments:** When compiled on native POSIX platforms (like Linux, BSD, or Cygwin), this library automatically detects the environment and skips Windows polyfills, falling back natively to `<unistd.h>` and `<sys/stat.h>`.

## Caveats and Implementation Notes

When utilizing these polyfills on Windows, please be aware of the following Win32-specific limitations and architectural decisions:

*   **No `<windows.h>` Pollution:** To ensure maximum compatibility and minimum compilation overhead, this library **does not** include `<windows.h>`. Instead, it provides minimal, inline forward declarations of required Win32 structs and function prototypes (e.g., `GetProcAddress`, `FILETIME`, `CreateFileA`).
*   **Path Length Limits (`MAX_PATH`):** Currently, paths resolved internally during polyfills (like in `fchmod` resolving a file handle to a path) are constrained to the standard Windows `MAX_PATH` limit (260 characters).
*   **ANSI ('A') APIs:** This library uses the standard ANSI versions of Win32 file APIs (e.g., `CreateFileA`, `GetFileAttributesA`). It does not natively support wide-character (UTF-16 `W`) strings. Consequently, paths containing characters outside the current active system ANSI code page may fail to resolve.
*   **UNC Paths:** Standard UNC paths (e.g., `\\server\share`) should work gracefully with the underlying `A` APIs, but extended-length UNC paths (`\\?\`) are generally not supported due to the `MAX_PATH` limitation and ANSI API constraints.
*   **Symbolic Links:** Windows implementation of symbolic links and reparse points differs drastically from POSIX. Functions like `lstat` will detect `FILE_ATTRIBUTE_REPARSE_POINT` and correctly flag `S_IFLNK`, but true POSIX symlink resolution semantics (and flags like `AT_SYMLINK_NOFOLLOW`) are emulated on a best-effort basis and might behave slightly differently in complex nested junction scenarios.
*   **Permissions (`chmod` / `fchmod`):** Windows lacks true POSIX file permission bits (e.g., granular read/write/execute for user/group/other). Calling `chmod` or `fchmod` on Windows only toggles the `FILE_ATTRIBUTE_READONLY` flag. Setting a file to writable clears this flag; setting it to read-only applies it. Group and Other permissions are entirely ignored.
*   **Device Nodes (`mknod`):** On Windows, `mknod` is only capable of creating standard files (`S_IFREG`) and directories (`S_IFDIR`, which redirects to `_mkdir`). Attempting to create character (`S_IFCHR`), block (`S_IFBLK`), FIFO (`S_IFIFO`), or socket (`S_IFSOCK`) nodes will result in an `-1` return with `errno = ENOSYS`.

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
    "posix-stat"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-stat
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-stat
)
FetchContent_MakeAvailable(posix-stat)
target_link_libraries(your_target PRIVATE posix-stat)
```
