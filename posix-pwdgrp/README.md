# posix-pwdgrp

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `endgrent` (Polyfilled with Win32 API)
- [x] `getgrent` (Polyfilled with Win32 API)
- [x] `getgrgid` (Polyfilled with LookupAccountSid)
- [x] `getgrgid_r` (Polyfilled with Win32 API)
- [x] `getgrnam` (Polyfilled with LookupAccountName)
- [x] `getgrnam_r` (Polyfilled with Win32 API)
- [x] `setgrent` (Polyfilled with Win32 API)
- [x] `endpwent` (Polyfilled with Win32 API)
- [x] `getpwent` (Polyfilled with Win32 API)
- [x] `getpwnam` (Polyfilled with LookupAccountName)
- [x] `getpwnam_r` (Polyfilled with Win32 API)
- [x] `getpwuid` (Polyfilled with LookupAccountSid)
- [x] `getpwuid_r` (Polyfilled with Win32 API)
- [x] `setpwent` (Polyfilled with Win32 API)

## Tested Environments

This library has been rigorously tested across multiple toolchains to ensure 100% C89 strictness, zero warnings, and memory-safe operation on both legacy and modern compilers.

| Environment | Compiler | Version | Warning Level | Status |
| :--- | :--- | :--- | :--- | :--- |
| **MSVC 2005** | `cl.exe` | v14.00 (x86) | `/W3 /WX` (Strict C89) | :white_check_mark: Pass |
| **MSVC 2022** | `cl.exe` | v19.3x | Default / `/W4` | :white_check_mark: Pass |
| **MSVC 2026** | `cl.exe` | v19.4x | Default / `/W4` | :white_check_mark: Pass |
| **MinGW (MSYS2)** | `gcc.exe` | Latest | `-Wall -Werror` | :white_check_mark: Pass |
| **Cygwin** | `gcc.exe` | Latest | `-Wall -Werror` | :white_check_mark: Pass |

## Implementation Notes & Caveats

To adapt Windows NT security concepts (SIDs) to POSIX user/group semantics, several design choices and compromises were necessary:

*   **UID/GID Mapping:** POSIX uses numeric `uid_t` and `gid_t`. Windows uses variable-length Security Identifiers (SIDs). This polyfill derives the numeric ID by taking the **last SubAuthority (the Relative ID or RID)** of the SID. This generally works well for local accounts and well-known groups, but may lead to collisions in complex multi-domain Active Directory setups if different domains use the same RIDs.
*   **String Encoding:** The Windows native APIs (`NetUserGetInfo`, `NetLocalGroupEnum`, etc.) return UTF-16 (`WCHAR`) strings. This library automatically converts these to **UTF-8** (`CP_UTF8`) for use with the standard POSIX `char *` fields. 
*   **Passwords:** The `pw_passwd` and `gr_passwd` fields will always return a dummy value (e.g., `*`). Windows does not expose user or group passwords in this manner.
*   **Thread Safety (`_r` variants):** The non-reentrant functions (e.g., `getpwnam`, `getpwuid`) return pointers to thread-local static buffers (`__declspec(thread)` or `__thread`). The `_r` variants use the user-supplied buffers.
*   **Buffer Size Constraints:** The internal static buffers are sized at 4096 bytes (`PWD_BUFFER_SIZE`). When using the `_r` functions, you must supply a buffer large enough to hold the structures, their internal string pointers, and the string data itself. If a string exceeds the buffer size, `ERANGE` is returned.
*   **File Paths & UNC:** User profile directories (`pw_dir`) are parsed from `usri4_home_dir`. If no directory is defined, it defaults to `C:\`. Be aware that Windows paths may exceed standard `MAX_PATH` (260 chars) or use UNC paths (`\\Server\Share`); ensure your application using these structs can handle Windows-style paths.
*   **Shells:** Windows does not have a direct concept of a default user shell like `/bin/bash` in POSIX. The `pw_shell` field will always return `"cmd.exe"`.
*   **Dependencies:** This library explicitly requires linking against `netapi32.lib` and `advapi32.lib` on Windows. This is handled automatically via `#pragma comment(lib, ...)` on MSVC, but must be specified manually (e.g., `-lnetapi32 -ladvapi32`) on MinGW/GCC setups.

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
    "posix-pwdgrp"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-pwdgrp
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-pwdgrp
)
FetchContent_MakeAvailable(posix-pwdgrp)
target_link_libraries(your_target PRIVATE posix-pwdgrp)
```
