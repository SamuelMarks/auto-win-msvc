# posix-ipc

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `ftok` (Polyfilled using `GetFileInformationByHandle` for unique file identification)
- [x] `msgctl` (Polyfilled using Win32 API)
- [x] `msgget` (Polyfilled using Win32 API)
- [x] `msgrcv` (Polyfilled using Win32 API)
- [x] `msgsnd` (Polyfilled using Win32 API)
- [x] `semctl` (Polyfilled using Win32 API)
- [x] `semget` (Polyfilled using Win32 API)
- [x] `semop` (Polyfilled using Win32 API)
- [x] `shmat` (Polyfilled using Win32 API)
- [x] `shmctl` (Polyfilled using Win32 API)
- [x] `shmdt` (Polyfilled using Win32 API)
- [x] `shmget` (Polyfilled using Win32 API)

## Tested Environments

The library has been rigorously tested and confirmed to build and pass all tests (with 0 warnings) in the following environments:

| Environment | Compiler / Toolchain | Status | Notes |
| :--- | :--- | :--- | :--- |
| **MSVC 2005** | `cl.exe` v14.0.50727 | ✅ Pass | Full C89 compliance. Disabled ancient Windows SDK macro warnings locally. |
| **MSVC 2022** | `cl.exe` v19.30+ | ✅ Pass | Clean build under `/W4 /WX`. Uses safe CRT `_s` string functions. |
| **MSVC 2026** | `cl.exe` v19.44+ | ✅ Pass | Clean build under `/W4 /WX`. Uses safe CRT `_s` string functions. |
| **MinGW-w64** | `gcc` v13.2.0 | ✅ Pass | Native `windows.h` environment fallback works perfectly. |
| **Cygwin** | `gcc` v13.4.0 | ✅ Pass | Natively uses `<sys/ipc.h>` via OS daemon (`cygserver`). |

## Implementation Caveats & Limitations

While this library faithfully replicates the System V IPC interfaces for Windows, developers should be aware of the following implications regarding its polyfill architecture:

*   **Path Length & Unicode:** The `ftok` implementation relies on `CreateFileA`. It does not currently support wide-character (Unicode) paths (`CreateFileW`) or extended-length paths (prefixing with `\\?\`). Paths are subject to the standard Windows `MAX_PATH` limit (260 characters).
*   **UNC Paths:** `ftok` may produce inconsistent keys if evaluated over mapped network drives vs. UNC paths (`\\server\share\file`) depending on how the SMB client reports the `nFileIndexLow`/`High` structure. It is highly recommended to use local, absolute paths for robust key generation.
*   **Process Isolation:** System V IPC inherently assumes a global, system-wide namespace. This polyfill simulates that namespace utilizing named Windows Mutexes, Events, and File Mapping Objects prefixed with `Local\SYSV_...`. Because they use the `Local\` object namespace, resources are isolated per user session (e.g., processes in an elevated Admin session might not see the IPC objects created by an unelevated session).
*   **Permissions (mode):** The `msgflg`, `shmflg`, and `semflg` permission bits (e.g., `0666`) are accepted by the API for POSIX compliance but are **not** currently enforced by the underlying Windows Security Descriptors. All objects are created with default security attributes (`NULL`), granting full access to the creating user.
*   **Limits:** The library currently enforces a hardcoded limit of `256` simultaneous active objects per IPC type (`MAX_SHM`, `MAX_SEM`, `MAX_MSG`).
*   **Message Queues:** The `msgflg & MSG_NOERROR` truncation logic relies on safe byte copying but may not identically match edge-case `errno` values provided by a true Linux kernel when bounds are exceeded. 

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
    "posix-ipc"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-ipc
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-ipc
)
FetchContent_MakeAvailable(posix-ipc)
target_link_libraries(your_target PRIVATE posix-ipc)
```
